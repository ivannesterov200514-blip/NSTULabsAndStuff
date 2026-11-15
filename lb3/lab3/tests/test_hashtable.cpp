// Boost.Test: хеш-функции, ChainTable (цепочки) и CuckooTable (кукушка).
#include <boost/test/unit_test.hpp>

#include <set>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "capture.h"
#include "hashtable.h"

using std::string;
using Pairs = std::vector<std::pair<string, string>>;

namespace {

const string kEmptyTable = "Хеш-таблица пуста\n";

// Проверка, что таблица содержит ровно ожидаемые пары (в любом порядке).
void check_same_content(const std::unordered_map<string, string>& ref, const Pairs& all) {
    BOOST_CHECK_EQUAL(all.size(), ref.size());
    std::set<string> seen;
    for (const auto& p : all) {
        BOOST_CHECK(seen.insert(p.first).second);   // нет дублей ключей
        auto it = ref.find(p.first);
        BOOST_REQUIRE(it != ref.end());
        BOOST_CHECK_EQUAL(p.second, it->second);
    }
}

}  // namespace

// =====================================================================
BOOST_AUTO_TEST_SUITE(HashFunctions)

BOOST_AUTO_TEST_CASE(known_values) {
    BOOST_CHECK_EQUAL(hash1("a", 11), 9);
    BOOST_CHECK_EQUAL(hash1("a", 101), 97);
    BOOST_CHECK_EQUAL(hash1("apple", 101), 29);
    BOOST_CHECK_EQUAL(hash1("abc", 101), 0);
    BOOST_CHECK_EQUAL(hash2("a", 11), 2);
    BOOST_CHECK_EQUAL(hash2("a", 101), 4);
    BOOST_CHECK_EQUAL(hash2("apple", 101), 91);
    BOOST_CHECK_EQUAL(hash2("abc", 101), 74);
}

BOOST_AUTO_TEST_CASE(empty_key_starts_from_initial_value) {
    BOOST_CHECK_EQUAL(hash1("", 101), 0);
    BOOST_CHECK_EQUAL(hash2("", 101), 7);
}

BOOST_AUTO_TEST_CASE(non_positive_modulus_returns_zero) {
    BOOST_CHECK_EQUAL(hash1("abc", 0), 0);
    BOOST_CHECK_EQUAL(hash2("abc", 0), 0);
    BOOST_CHECK_EQUAL(hash1("abc", -5), 0);
    BOOST_CHECK_EQUAL(hash2("abc", -5), 0);
}

BOOST_AUTO_TEST_CASE(modulus_one_always_gives_zero) {
    for (const char* k : {"a", "apple", "banana", "key"}) {
        BOOST_CHECK_EQUAL(hash1(k, 1), 0);
        BOOST_CHECK_EQUAL(hash2(k, 1), 0);
    }
}

BOOST_AUTO_TEST_CASE(result_is_deterministic_and_in_range) {
    for (int m : {2, 11, 101, 1009}) {
        for (int i = 0; i < 200; ++i) {
            string key = "key" + std::to_string(i);
            int h1 = hash1(key, m);
            int h2 = hash2(key, m);
            BOOST_CHECK(h1 >= 0 && h1 < m);
            BOOST_CHECK(h2 >= 0 && h2 < m);
            BOOST_CHECK_EQUAL(h1, hash1(key, m));
            BOOST_CHECK_EQUAL(h2, hash2(key, m));
        }
    }
}

BOOST_AUTO_TEST_CASE(two_hash_functions_differ) {
    int different = 0;
    for (int i = 0; i < 100; ++i) {
        string key = "key" + std::to_string(i);
        if (hash1(key, 1009) != hash2(key, 1009)) ++different;
    }
    BOOST_CHECK(different > 50);
}

BOOST_AUTO_TEST_SUITE_END()

// =====================================================================
BOOST_AUTO_TEST_SUITE(ChainTableTests)

BOOST_AUTO_TEST_CASE(new_table_is_empty) {
    ChainTable t;
    BOOST_CHECK_EQUAL(t.ct_getsz(), 0);
    BOOST_CHECK_EQUAL(t.ct_getcap(), 101);
    BOOST_CHECK(t.ct_get_all().empty());
}

BOOST_AUTO_TEST_CASE(non_positive_capacity_falls_back_to_default) {
    ChainTable zero(0);
    ChainTable negative(-7);
    BOOST_CHECK_EQUAL(zero.ct_getcap(), 101);
    BOOST_CHECK_EQUAL(negative.ct_getcap(), 101);
}

BOOST_AUTO_TEST_CASE(custom_capacity_is_kept) {
    ChainTable t(17);
    BOOST_CHECK_EQUAL(t.ct_getcap(), 17);
}

BOOST_AUTO_TEST_CASE(put_and_get) {
    ChainTable t;
    t.ct_put("apple", "red");
    t.ct_put("banana", "yellow");
    string value;
    BOOST_CHECK(t.ct_get("apple", value));
    BOOST_CHECK_EQUAL(value, "red");
    BOOST_CHECK(t.ct_get("banana", value));
    BOOST_CHECK_EQUAL(value, "yellow");
    BOOST_CHECK_EQUAL(t.ct_getsz(), 2);
}

BOOST_AUTO_TEST_CASE(put_existing_key_updates_value_without_growing) {
    ChainTable t;
    t.ct_put("apple", "red");
    t.ct_put("apple", "green");
    string value;
    BOOST_CHECK(t.ct_get("apple", value));
    BOOST_CHECK_EQUAL(value, "green");
    BOOST_CHECK_EQUAL(t.ct_getsz(), 1);
}

BOOST_AUTO_TEST_CASE(get_missing_key_leaves_output_untouched) {
    ChainTable t;
    t.ct_put("apple", "red");
    string value = "untouched";
    BOOST_CHECK(!t.ct_get("nope", value));
    BOOST_CHECK_EQUAL(value, "untouched");
}

BOOST_AUTO_TEST_CASE(empty_value_is_allowed) {
    ChainTable t;
    t.ct_put("k", "");
    string value = "x";
    BOOST_CHECK(t.ct_get("k", value));
    BOOST_CHECK_EQUAL(value, "");
}

BOOST_AUTO_TEST_CASE(remove_existing_and_missing_key) {
    ChainTable t;
    t.ct_put("apple", "red");
    BOOST_CHECK(t.ct_remove("apple"));
    BOOST_CHECK(!t.ct_remove("apple"));
    BOOST_CHECK(!t.ct_remove("never_existed"));
    BOOST_CHECK_EQUAL(t.ct_getsz(), 0);
    string value;
    BOOST_CHECK(!t.ct_get("apple", value));
}

BOOST_AUTO_TEST_CASE(colliding_keys_share_a_bucket_and_stay_separate) {
    // "apple" и "key" дают hash1 == 29 при емкости 101
    BOOST_REQUIRE_EQUAL(hash1("apple", 101), hash1("key", 101));
    ChainTable t;
    t.ct_put("apple", "1");
    t.ct_put("key", "2");
    string value;
    BOOST_CHECK(t.ct_get("apple", value));
    BOOST_CHECK_EQUAL(value, "1");
    BOOST_CHECK(t.ct_get("key", value));
    BOOST_CHECK_EQUAL(value, "2");
    BOOST_CHECK(t.ct_remove("apple"));
    BOOST_CHECK(t.ct_get("key", value));
    BOOST_CHECK_EQUAL(value, "2");
}

BOOST_AUTO_TEST_CASE(single_bucket_table_remove_head_middle_tail) {
    ChainTable t(1);
    for (int i = 0; i < 5; ++i) t.ct_put("k" + std::to_string(i), "v" + std::to_string(i));
    BOOST_CHECK(t.ct_remove("k4"));   // голова цепочки (вставка идет в голову)
    BOOST_CHECK(t.ct_remove("k2"));   // середина
    BOOST_CHECK(t.ct_remove("k0"));   // хвост
    BOOST_CHECK_EQUAL(t.ct_getsz(), 2);
    string value;
    BOOST_CHECK(t.ct_get("k1", value));
    BOOST_CHECK(t.ct_get("k3", value));
    BOOST_CHECK(!t.ct_get("k2", value));
}

BOOST_AUTO_TEST_CASE(get_all_returns_every_pair) {
    ChainTable t;
    std::unordered_map<string, string> ref;
    for (int i = 0; i < 50; ++i) {
        string k = "key" + std::to_string(i);
        string v = "val" + std::to_string(i);
        t.ct_put(k, v);
        ref[k] = v;
    }
    check_same_content(ref, t.ct_get_all());
}

BOOST_AUTO_TEST_CASE(get_all_in_single_bucket_goes_from_newest_to_oldest) {
    ChainTable t(1);
    t.ct_put("k0", "v0");
    t.ct_put("k1", "v1");
    t.ct_put("k2", "v2");
    Pairs expected = {{"k2", "v2"}, {"k1", "v1"}, {"k0", "v0"}};
    BOOST_CHECK(t.ct_get_all() == expected);
}

BOOST_AUTO_TEST_CASE(print_empty_table) {
    ChainTable t;
    string out = capture_output([&] { t.ct_print(); });
    BOOST_CHECK_EQUAL(out, kEmptyTable);
}

BOOST_AUTO_TEST_CASE(print_shows_only_non_empty_buckets) {
    ChainTable t;
    t.ct_put("apple", "red");
    string out = capture_output([&] { t.ct_print(); });
    BOOST_CHECK_EQUAL(out,
                      "Хеш-таблица (цепочки):\n"
                      "[29] apple=red -> nullptr\n");
}

BOOST_AUTO_TEST_CASE(print_collision_chain_newest_first) {
    ChainTable t;
    t.ct_put("apple", "1");
    t.ct_put("key", "2");
    string out = capture_output([&] { t.ct_print(); });
    BOOST_CHECK_EQUAL(out,
                      "Хеш-таблица (цепочки):\n"
                      "[29] key=2 -> apple=1 -> nullptr\n");
}

BOOST_AUTO_TEST_CASE(print_after_removing_everything_says_empty) {
    ChainTable t;
    t.ct_put("a", "1");
    t.ct_remove("a");
    string out = capture_output([&] { t.ct_print(); });
    BOOST_CHECK_EQUAL(out, kEmptyTable);
}

BOOST_AUTO_TEST_CASE(table_cannot_be_copied) {
    BOOST_CHECK(!std::is_copy_constructible<ChainTable>::value);
    BOOST_CHECK(!std::is_copy_assignable<ChainTable>::value);
}

BOOST_AUTO_TEST_CASE(random_operations_match_unordered_map) {
    std::mt19937 gen(2024);
    ChainTable t(7);
    std::unordered_map<string, string> ref;
    for (int i = 0; i < 5000; ++i) {
        string key = "k" + std::to_string(gen() % 300);
        string val = "v" + std::to_string(gen() % 1000);
        switch (gen() % 3) {
            case 0:
                t.ct_put(key, val);
                ref[key] = val;
                break;
            case 1: {
                string got;
                bool found = t.ct_get(key, got);
                auto it = ref.find(key);
                BOOST_REQUIRE_EQUAL(found, it != ref.end());
                if (found) BOOST_REQUIRE_EQUAL(got, it->second);
                break;
            }
            default:
                BOOST_REQUIRE_EQUAL(t.ct_remove(key), ref.erase(key) == 1);
                break;
        }
        BOOST_REQUIRE_EQUAL(static_cast<size_t>(t.ct_getsz()), ref.size());
    }
    check_same_content(ref, t.ct_get_all());
}

BOOST_AUTO_TEST_SUITE_END()

// =====================================================================
BOOST_AUTO_TEST_SUITE(CuckooTableTests)

BOOST_AUTO_TEST_CASE(new_table_is_empty) {
    CuckooTable t;
    BOOST_CHECK_EQUAL(t.cu_getsz(), 0);
    BOOST_CHECK_EQUAL(t.cu_getcap(), 11);
    BOOST_CHECK_EQUAL(t.cu_getgrows(), 0);
    BOOST_CHECK_EQUAL(t.cu_getkicks(), 0u);
    BOOST_CHECK(t.cu_get_all().empty());
}

BOOST_AUTO_TEST_CASE(non_positive_capacity_falls_back_to_default) {
    CuckooTable zero(0);
    CuckooTable negative(-3);
    BOOST_CHECK_EQUAL(zero.cu_getcap(), 11);
    BOOST_CHECK_EQUAL(negative.cu_getcap(), 11);
}

BOOST_AUTO_TEST_CASE(put_and_get) {
    CuckooTable t;
    t.cu_put("apple", "red");
    t.cu_put("banana", "yellow");
    string value;
    BOOST_CHECK(t.cu_get("apple", value));
    BOOST_CHECK_EQUAL(value, "red");
    BOOST_CHECK(t.cu_get("banana", value));
    BOOST_CHECK_EQUAL(value, "yellow");
    BOOST_CHECK_EQUAL(t.cu_getsz(), 2);
}

BOOST_AUTO_TEST_CASE(put_existing_key_updates_value_without_growing) {
    CuckooTable t;
    t.cu_put("a", "1");
    t.cu_put("a", "2");
    string value;
    BOOST_CHECK(t.cu_get("a", value));
    BOOST_CHECK_EQUAL(value, "2");
    BOOST_CHECK_EQUAL(t.cu_getsz(), 1);
}

BOOST_AUTO_TEST_CASE(updating_a_key_stored_in_second_table) {
    // "a" и "k15" совпадают по hash1 при емкости 11, поэтому один из них окажется в T2
    BOOST_REQUIRE_EQUAL(hash1("a", 11), hash1("k15", 11));
    CuckooTable t;
    t.cu_put("a", "1");
    t.cu_put("k15", "2");
    t.cu_put("a", "10");
    t.cu_put("k15", "20");
    string value;
    BOOST_CHECK(t.cu_get("a", value));
    BOOST_CHECK_EQUAL(value, "10");
    BOOST_CHECK(t.cu_get("k15", value));
    BOOST_CHECK_EQUAL(value, "20");
    BOOST_CHECK_EQUAL(t.cu_getsz(), 2);
}

BOOST_AUTO_TEST_CASE(get_missing_key_leaves_output_untouched) {
    CuckooTable t;
    t.cu_put("a", "1");
    string value = "untouched";
    BOOST_CHECK(!t.cu_get("zzz", value));
    BOOST_CHECK_EQUAL(value, "untouched");
}

BOOST_AUTO_TEST_CASE(empty_key_is_reserved_and_ignored) {
    CuckooTable t;
    t.cu_put("", "x");
    string value = "untouched";
    BOOST_CHECK_EQUAL(t.cu_getsz(), 0);
    BOOST_CHECK(!t.cu_get("", value));
    BOOST_CHECK(!t.cu_remove(""));
    BOOST_CHECK_EQUAL(value, "untouched");
}

BOOST_AUTO_TEST_CASE(remove_existing_and_missing_key) {
    CuckooTable t;
    t.cu_put("a", "1");
    BOOST_CHECK(t.cu_remove("a"));
    BOOST_CHECK(!t.cu_remove("a"));
    BOOST_CHECK_EQUAL(t.cu_getsz(), 0);
    string value;
    BOOST_CHECK(!t.cu_get("a", value));
}

BOOST_AUTO_TEST_CASE(removed_key_can_be_put_again) {
    CuckooTable t;
    t.cu_put("a", "1");
    t.cu_remove("a");
    t.cu_put("a", "2");
    string value;
    BOOST_CHECK(t.cu_get("a", value));
    BOOST_CHECK_EQUAL(value, "2");
    BOOST_CHECK_EQUAL(t.cu_getsz(), 1);
}

BOOST_AUTO_TEST_CASE(collision_in_first_table_kicks_the_old_key) {
    BOOST_REQUIRE_EQUAL(hash1("a", 11), hash1("k15", 11));
    CuckooTable t;
    t.cu_put("a", "1");
    BOOST_CHECK_EQUAL(t.cu_getkicks(), 0u);
    t.cu_put("k15", "2");
    BOOST_CHECK(t.cu_getkicks() >= 1u);
    string value;
    BOOST_CHECK(t.cu_get("a", value));
    BOOST_CHECK(t.cu_get("k15", value));
    BOOST_CHECK_EQUAL(t.cu_getsz(), 2);
}

BOOST_AUTO_TEST_CASE(table_grows_and_keeps_every_key) {
    CuckooTable t(3);
    std::unordered_map<string, string> ref;
    for (int i = 0; i < 500; ++i) {
        string k = "key" + std::to_string(i);
        string v = "val" + std::to_string(i);
        t.cu_put(k, v);
        ref[k] = v;
    }
    BOOST_CHECK_EQUAL(t.cu_getsz(), 500);
    BOOST_CHECK(t.cu_getgrows() > 0);
    BOOST_CHECK(t.cu_getcap() > 3);
    BOOST_CHECK(t.cu_getkicks() > 0u);
    for (const auto& kv : ref) {
        string value;
        BOOST_REQUIRE(t.cu_get(kv.first, value));
        BOOST_CHECK_EQUAL(value, kv.second);
    }
    check_same_content(ref, t.cu_get_all());
}

BOOST_AUTO_TEST_CASE(capacity_follows_the_doubling_formula) {
    // каждый рост меняет емкость как cap -> 2 * cap + 1, то есть 3 -> 7 -> 15 -> ...
    CuckooTable t(3);
    for (int i = 0; i < 300; ++i) t.cu_put("key" + std::to_string(i), "v");
    BOOST_REQUIRE(t.cu_getgrows() > 0);
    BOOST_CHECK_EQUAL(t.cu_getcap(), (4 << t.cu_getgrows()) - 1);
}

BOOST_AUTO_TEST_CASE(remove_half_of_the_keys_after_growth) {
    CuckooTable t(3);
    for (int i = 0; i < 200; ++i) t.cu_put("key" + std::to_string(i), "v");
    for (int i = 0; i < 200; i += 2) BOOST_CHECK(t.cu_remove("key" + std::to_string(i)));
    BOOST_CHECK_EQUAL(t.cu_getsz(), 100);
    string value;
    for (int i = 0; i < 200; ++i) {
        BOOST_CHECK_EQUAL(t.cu_get("key" + std::to_string(i), value), i % 2 == 1);
    }
}

BOOST_AUTO_TEST_CASE(get_all_lists_first_table_then_second) {
    CuckooTable t;
    t.cu_put("a", "1");
    Pairs expected = {{"a", "1"}};
    BOOST_CHECK(t.cu_get_all() == expected);
}

BOOST_AUTO_TEST_CASE(print_empty_table) {
    CuckooTable t;
    string out = capture_output([&] { t.cu_print(); });
    BOOST_CHECK_EQUAL(out, kEmptyTable);
}

BOOST_AUTO_TEST_CASE(print_shows_slot_positions) {
    CuckooTable t;
    t.cu_put("a", "1");   // hash1("a", 11) == 9
    string out = capture_output([&] { t.cu_print(); });
    BOOST_CHECK_EQUAL(out,
                      "Хеш-таблица (кукушка):\n"
                      "T1[9] a=1\n");
}

BOOST_AUTO_TEST_CASE(print_shows_second_table_after_kick) {
    BOOST_REQUIRE_EQUAL(hash1("a", 11), hash1("k15", 11));
    CuckooTable t;
    t.cu_put("a", "1");
    t.cu_put("k15", "2");   // вытесняет "a" в T2: hash2("a", 11) == 2
    string out = capture_output([&] { t.cu_print(); });
    BOOST_CHECK_EQUAL(out,
                      "Хеш-таблица (кукушка):\n"
                      "T1[9] k15=2\n"
                      "T2[2] a=1\n");
}

BOOST_AUTO_TEST_CASE(table_cannot_be_copied) {
    BOOST_CHECK(!std::is_copy_constructible<CuckooTable>::value);
    BOOST_CHECK(!std::is_copy_assignable<CuckooTable>::value);
}

BOOST_AUTO_TEST_CASE(random_operations_match_unordered_map) {
    std::mt19937 gen(777);
    CuckooTable t(3);
    std::unordered_map<string, string> ref;
    for (int i = 0; i < 5000; ++i) {
        string key = "k" + std::to_string(gen() % 300);
        string val = "v" + std::to_string(gen() % 1000);
        switch (gen() % 3) {
            case 0:
                t.cu_put(key, val);
                ref[key] = val;
                break;
            case 1: {
                string got;
                bool found = t.cu_get(key, got);
                auto it = ref.find(key);
                BOOST_REQUIRE_EQUAL(found, it != ref.end());
                if (found) BOOST_REQUIRE_EQUAL(got, it->second);
                break;
            }
            default:
                BOOST_REQUIRE_EQUAL(t.cu_remove(key), ref.erase(key) == 1);
                break;
        }
        BOOST_REQUIRE_EQUAL(static_cast<size_t>(t.cu_getsz()), ref.size());
    }
    check_same_content(ref, t.cu_get_all());
}

BOOST_AUTO_TEST_SUITE_END()
