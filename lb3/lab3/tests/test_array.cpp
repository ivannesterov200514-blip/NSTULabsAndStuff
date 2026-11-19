#include <catch2/catch_test_macros.hpp>

#include <string>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "array.h"
#include "capture.h"

using std::string;
using Strings = std::vector<string>;

namespace {

const string kBadIndex = "Ошибка: Такого индекса нет\n";

Strings contents(Arr& a) {
    Strings res;
    for (int i = 0; i < a.arr_getsz(); ++i) res.push_back(a.arr_search(i));
    return res;
}

}  

TEST_CASE("Arr: new array is empty", "[array]") {
    Arr a;
    REQUIRE(a.arr_getsz() == 0);
    REQUIRE(contents(a).empty());
}

TEST_CASE("Arr: insert_tail appends in order and grows past capacity", "[array]") {
    Arr a(1);
    for (int i = 0; i < 10; ++i) a.arr_insert_tail(std::to_string(i));
    REQUIRE(a.arr_getsz() == 10);
    Strings expected{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    REQUIRE(contents(a) == expected);
}

TEST_CASE("Arr: arr_search", "[array]") {
    SECTION("valid indexes") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_insert_tail("b");
        string first;
        string last;
        string out = capture_output([&] {
            first = a.arr_search(0);
            last = a.arr_search(1);
        });
        REQUIRE(first == "a");
        REQUIRE(last == "b");
        REQUIRE(out.empty());
    }
    SECTION("index equal to size") {
        Arr a;
        a.arr_insert_tail("a");
        string result = "unchanged";
        string out = capture_output([&] { result = a.arr_search(1); });
        REQUIRE(result.empty());
        REQUIRE(out == kBadIndex);
    }
    SECTION("negative index") {
        Arr a;
        a.arr_insert_tail("a");
        string result = "unchanged";
        string out = capture_output([&] { result = a.arr_search(-1); });
        REQUIRE(result.empty());
        REQUIRE(out == kBadIndex);
    }
    SECTION("empty array") {
        Arr a;
        string out = capture_output([&] { a.arr_search(0); });
        REQUIRE(out == kBadIndex);
    }
}

TEST_CASE("Arr: arr_insert_idx", "[array]") {
    SECTION("at the beginning") {
        Arr a;
        a.arr_insert_tail("b");
        a.arr_insert_idx("a", 0);
        REQUIRE(contents(a) == Strings{"a", "b"});
    }
    SECTION("in the middle") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_insert_tail("c");
        a.arr_insert_idx("b", 1);
        REQUIRE(contents(a) == Strings{"a", "b", "c"});
    }
    SECTION("at the end (index equals size)") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_insert_idx("b", 1);
        REQUIRE(contents(a) == Strings{"a", "b"});
    }
    SECTION("into an empty array") {
        Arr a;
        a.arr_insert_idx("a", 0);
        REQUIRE(contents(a) == Strings{"a"});
    }
    SECTION("index greater than size is rejected") {
        Arr a;
        a.arr_insert_tail("a");
        string out = capture_output([&] { a.arr_insert_idx("x", 2); });
        REQUIRE(out == kBadIndex);
        REQUIRE(a.arr_getsz() == 1);
    }
    SECTION("negative index is rejected") {
        Arr a;
        string out = capture_output([&] { a.arr_insert_idx("x", -1); });
        REQUIRE(out == kBadIndex);
        REQUIRE(a.arr_getsz() == 0);
    }
    SECTION("repeated inserts at the front force reallocation") {
        Arr a(1);
        for (int i = 0; i < 8; ++i) a.arr_insert_idx(std::to_string(i), 0);
        REQUIRE(contents(a) == Strings{"7", "6", "5", "4", "3", "2", "1", "0"});
    }
}

TEST_CASE("Arr: arr_delete_idx", "[array]") {
    SECTION("first element") {
        Arr a;
        for (const char* s : {"a", "b", "c"}) a.arr_insert_tail(s);
        a.arr_delete_idx(0);
        REQUIRE(contents(a) == Strings{"b", "c"});
    }
    SECTION("middle element") {
        Arr a;
        for (const char* s : {"a", "b", "c"}) a.arr_insert_tail(s);
        a.arr_delete_idx(1);
        REQUIRE(contents(a) == Strings{"a", "c"});
    }
    SECTION("last element") {
        Arr a;
        for (const char* s : {"a", "b", "c"}) a.arr_insert_tail(s);
        a.arr_delete_idx(2);
        REQUIRE(contents(a) == Strings{"a", "b"});
    }
    SECTION("index equal to size is rejected") {
        Arr a;
        a.arr_insert_tail("a");
        string out = capture_output([&] { a.arr_delete_idx(1); });
        REQUIRE(out == kBadIndex);
        REQUIRE(a.arr_getsz() == 1);
    }
    SECTION("negative index is rejected") {
        Arr a;
        a.arr_insert_tail("a");
        string out = capture_output([&] { a.arr_delete_idx(-1); });
        REQUIRE(out == kBadIndex);
        REQUIRE(a.arr_getsz() == 1);
    }
    SECTION("empty array is rejected") {
        Arr a;
        string out = capture_output([&] { a.arr_delete_idx(0); });
        REQUIRE(out == kBadIndex);
    }
    SECTION("array stays usable after becoming empty") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_delete_idx(0);
        REQUIRE(a.arr_getsz() == 0);
        a.arr_insert_tail("b");
        REQUIRE(contents(a) == Strings{"b"});
    }
}

TEST_CASE("Arr: arr_change_idx", "[array]") {
    SECTION("replaces the value and keeps the size") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_insert_tail("b");
        a.arr_change_idx("z", 1);
        REQUIRE(contents(a) == Strings{"a", "z"});
        REQUIRE(a.arr_getsz() == 2);
    }
    SECTION("index equal to size is rejected") {
        Arr a;
        a.arr_insert_tail("a");
        string out = capture_output([&] { a.arr_change_idx("z", 1); });
        REQUIRE(out == kBadIndex);
        REQUIRE(contents(a) == Strings{"a"});
    }
    SECTION("negative index is rejected") {
        Arr a;
        a.arr_insert_tail("a");
        string out = capture_output([&] { a.arr_change_idx("z", -1); });
        REQUIRE(out == kBadIndex);
    }
}

TEST_CASE("Arr: arr_resize keeps the existing elements", "[array]") {
    Arr a(2);
    a.arr_insert_tail("a");
    a.arr_insert_tail("b");
    a.arr_resize(100);
    REQUIRE(contents(a) == Strings{"a", "b"});
    a.arr_insert_tail("c");
    REQUIRE(contents(a) == Strings{"a", "b", "c"});
}

TEST_CASE("Arr: arr_print format", "[array]") {
    SECTION("empty") {
        Arr a;
        string out = capture_output([&] { a.arr_print(); });
        REQUIRE(out == "Массив: \n");
    }
    SECTION("with elements") {
        Arr a;
        a.arr_insert_tail("a");
        a.arr_insert_tail("b");
        string out = capture_output([&] { a.arr_print(); });
        REQUIRE(out == "Массив: a b \n");
    }
}

TEST_CASE("Arr: invalid resize is rejected without damaging elements", "[array][safety]") {
    Arr a;
    a.arr_insert_tail("one");
    a.arr_insert_tail("two");
    REQUIRE_THROWS_AS(a.arr_resize(1), std::invalid_argument);
    REQUIRE_THROWS_AS(a.arr_resize(0), std::invalid_argument);
    REQUIRE(contents(a) == Strings{"one", "two"});
}
