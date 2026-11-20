#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "capture.h"
#include "ulist.h"

using namespace std;

using Strings = vector<string>;

namespace {

const string kNotFound = "Ошибка: Не найдено\n";
const string kEmptyList = "Ошибка: Список пуст\n";
const string kLastRemoved = "Последний узел списка удален.\n";
const string kNothingBefore = "Ошибка: Перед элементом ничего нет\n";
const string kNothingAfter = "Ошибка: После элемента ничего нет\n";

Strings keys(const UList& l) {
    Strings res;
    for (const auto& p : l.ul_get_pairs()) res.push_back(p.first);
    return res;
}

class UListTest : public ::testing::Test {
protected:
    UList empty;
    UList abc;

    void SetUp() override {
        abc.ul_insert_tail("a");
        abc.ul_insert_tail("b");
        abc.ul_insert_tail("c");
    }
};

}  

TEST_F(UListTest, NewListIsEmpty) {
    EXPECT_EQ(empty.ul_getsz(), 0);
    EXPECT_TRUE(keys(empty).empty());
}

TEST_F(UListTest, InsertHeadOnEmptyList) {
    empty.ul_insert_head("x");
    EXPECT_EQ(keys(empty), (Strings{"x"}));
    EXPECT_EQ(empty.ul_getsz(), 1);
}

TEST_F(UListTest, InsertTailAppendsLast) {
    abc.ul_insert_tail("x");
    EXPECT_EQ(keys(abc), (Strings{"a", "b", "c", "x"}));
    EXPECT_EQ(abc.ul_getsz(), 4);
}

TEST_F(UListTest, InsertBeforeMiddleAndTail) {
    abc.ul_insert_before("b", "x");
    abc.ul_insert_before("c", "y");
    EXPECT_EQ(keys(abc), (Strings{"a", "x", "b", "y", "c"}));
    EXPECT_EQ(abc.ul_getsz(), 5);
}

TEST_F(UListTest, InsertBeforeMissingKeyPrintsError) {
    string out = capture_output([&] { abc.ul_insert_before("zzz", "x"); });
    EXPECT_EQ(out, kNotFound);
    EXPECT_EQ(keys(abc), (Strings{"a", "b", "c"}));
}

TEST_F(UListTest, InsertAfterHeadMiddleTail) {
    abc.ul_insert_after("a", "x");
    abc.ul_insert_after("b", "y");
    abc.ul_insert_after("c", "z");
    EXPECT_EQ(keys(abc), (Strings{"a", "x", "b", "y", "c", "z"}));
    EXPECT_EQ(abc.ul_getsz(), 6);
}

TEST_F(UListTest, DeleteHeadOfSingleElementReportsLastNode) {
    empty.ul_insert_head("x");
    string out = capture_output([&] { empty.ul_delete_head(); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_EQ(empty.ul_getsz(), 0);
}

TEST_F(UListTest, DeleteTailRemovesLast) {
    string out = capture_output([&] { abc.ul_delete_tail(); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(keys(abc), (Strings{"a", "b"}));
    EXPECT_EQ(abc.ul_getsz(), 2);
}

TEST_F(UListTest, DeleteBeforeHeadPrintsNothingBefore) {
    string out = capture_output([&] { abc.ul_delete_before("a"); });
    EXPECT_EQ(out, kNothingBefore);
    EXPECT_EQ(abc.ul_getsz(), 3);
}

TEST_F(UListTest, DeleteBeforeTailRemovesMiddle) {
    string out = capture_output([&] { abc.ul_delete_before("c"); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(keys(abc), (Strings{"a", "c"}));
    EXPECT_EQ(abc.ul_getsz(), 2);
}

TEST_F(UListTest, DeleteAfterHeadRemovesSecond) {
    string out = capture_output([&] { abc.ul_delete_after("a"); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(keys(abc), (Strings{"a", "c"}));
}

TEST_F(UListTest, DeleteAfterTailPrintsNothingAfter) {
    string out = capture_output([&] { abc.ul_delete_after("c"); });
    EXPECT_EQ(out, kNothingAfter);
    EXPECT_EQ(abc.ul_getsz(), 3);
}

TEST_F(UListTest, DeleteKeyTail) {
    abc.ul_delete_key("c");
    EXPECT_EQ(keys(abc), (Strings{"a", "b"}));
    EXPECT_EQ(abc.ul_getsz(), 2);

    abc.ul_insert_tail("d");
    EXPECT_EQ(keys(abc), (Strings{"a", "b", "d"}));
}

TEST_F(UListTest, DeleteKeyMissingPrintsNotFound) {
    string out = capture_output([&] { abc.ul_delete_key("zzz"); });
    EXPECT_EQ(out, kNotFound);
    EXPECT_EQ(abc.ul_getsz(), 3);
}

TEST_F(UListTest, SearchFoundPrintsAndReturnsKey) {
    string result;
    string out = capture_output([&] { result = abc.ul_search("b"); });
    EXPECT_EQ(result, "b");
    EXPECT_EQ(out, "b\n");
}

TEST_F(UListTest, SearchMissingReturnsEmptyString) {
    string result = "unchanged";
    string out = capture_output([&] { result = abc.ul_search("zzz"); });
    EXPECT_EQ(result, "");
    EXPECT_EQ(out, kNotFound);
}

TEST_F(UListTest, PrintRecMatchesPrintIter) {
    string rec = capture_output([&] { abc.ul_print_rec(); });
    string iter = capture_output([&] { abc.ul_print_iter(); });
    EXPECT_EQ(rec, iter);
}

TEST_F(UListTest, PrintOfEmptyListSaysEmpty) {
    string iter = capture_output([&] { empty.ul_print_iter(); });
    string rec = capture_output([&] { empty.ul_print_rec(); });
    EXPECT_EQ(iter, "Список пуст\n");
    EXPECT_EQ(rec, "Список пуст\n");
}

TEST_F(UListTest, GetValueFoundAndMissing) {
    empty.ul_insert_head("k", "v");
    string value;
    EXPECT_TRUE(empty.ul_get_value("k", value));
    EXPECT_EQ(value, "v");

    value = "untouched";
    EXPECT_FALSE(empty.ul_get_value("nope", value));
    EXPECT_EQ(value, "untouched");
}

TEST_F(UListTest, UpdateValue) {
    empty.ul_insert_head("k", "old");
    EXPECT_TRUE(empty.ul_update_value("k", "new"));
    EXPECT_FALSE(empty.ul_update_value("missing", "x"));
    string value;
    EXPECT_TRUE(empty.ul_get_value("k", value));
    EXPECT_EQ(value, "new");
    EXPECT_EQ(empty.ul_getsz(), 1);
}

TEST_F(UListTest, RemoveKeyHeadMiddleTailAndMissing) {
    empty.ul_insert_tail("a");
    empty.ul_insert_tail("b");
    empty.ul_insert_tail("c");
    empty.ul_insert_tail("d");
    EXPECT_TRUE(empty.ul_remove_key("a"));   
    EXPECT_TRUE(empty.ul_remove_key("c"));   
    EXPECT_TRUE(empty.ul_remove_key("d"));   
    EXPECT_FALSE(empty.ul_remove_key("zzz"));
    EXPECT_EQ(keys(empty), (Strings{"b"}));
    EXPECT_EQ(empty.ul_getsz(), 1);
    EXPECT_TRUE(empty.ul_remove_key("b"));
    EXPECT_FALSE(empty.ul_remove_key("b"));   
    EXPECT_EQ(empty.ul_getsz(), 0);
}

TEST_F(UListTest, PrintPairsFormat) {
    empty.ul_insert_head("k1", "v1");
    empty.ul_insert_head("k2", "v2");
    string out = capture_output([&] { empty.ul_print_pairs(); });
    EXPECT_EQ(out, "k2=v2 -> k1=v1 -> nullptr\n");

    UList nothing;
    out = capture_output([&] { nothing.ul_print_pairs(); });
    EXPECT_EQ(out, "nullptr\n");
}

TEST(UListLifetimeTest, LargeListIsDestroyedWithoutStackOverflow) {
    UList big;
    for (int i = 0; i < 100000; ++i) big.ul_insert_head(to_string(i));
    EXPECT_EQ(big.ul_getsz(), 100000);
}   
