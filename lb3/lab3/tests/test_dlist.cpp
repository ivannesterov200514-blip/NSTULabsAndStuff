#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "capture.h"
#include "dlist.h"

using namespace std;

using Strings = vector<string>;

namespace {

const string kNotFound = "Ошибка: Не найдено\n";
const string kEmptyList = "Ошибка: Список пуст\n";
const string kLastRemoved = "Последний узел списка удален.\n";
const string kNothingBefore = "Ошибка: Перед элементом ничего нет\n";
const string kNothingAfter = "Ошибка: После элемента ничего нет\n";

class DListTest : public ::testing::Test {
protected:
    DList empty;
    DList abc;

    void SetUp() override {
        abc.dl_insert_tail("a");
        abc.dl_insert_tail("b");
        abc.dl_insert_tail("c");
    }
};

}  

TEST_F(DListTest, NewListIsEmpty) {
    EXPECT_TRUE(empty.dl_items().empty());
}

TEST_F(DListTest, InsertHeadOnEmptyListSetsTail) {
    empty.dl_insert_head("x");
    EXPECT_EQ(empty.dl_items(), (Strings{"x"}));

    string out = capture_output([&] { empty.dl_delete_tail(); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(empty.dl_items().empty());
}

TEST_F(DListTest, InsertTailOnEmptyListSetsHead) {
    empty.dl_insert_tail("x");
    string out = capture_output([&] { empty.dl_delete_head(); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(empty.dl_items().empty());
}

TEST_F(DListTest, InsertBeforeHeadMiddleTail) {
    abc.dl_insert_before("a", "x");
    abc.dl_insert_before("b", "y");
    abc.dl_insert_before("c", "z");
    EXPECT_EQ(abc.dl_items(), (Strings{"x", "a", "y", "b", "z", "c"}));
}

TEST_F(DListTest, InsertBeforeMissingKeyPrintsError) {
    string out = capture_output([&] { abc.dl_insert_before("zzz", "x"); });
    EXPECT_EQ(out, kNotFound);
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "c"}));
}

TEST_F(DListTest, InsertAfterHeadMiddleTail) {
    abc.dl_insert_after("a", "x");
    abc.dl_insert_after("b", "y");
    abc.dl_insert_after("c", "z");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "x", "b", "y", "c", "z"}));
}

TEST_F(DListTest, InsertAfterTailUpdatesTail) {
    abc.dl_insert_after("c", "x");

    abc.dl_delete_tail();
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "c"}));
}

TEST_F(DListTest, DeleteHeadRemovesFirst) {
    string out = capture_output([&] { abc.dl_delete_head(); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(abc.dl_items(), (Strings{"b", "c"}));

    out = capture_output([&] { abc.dl_delete_before("b"); });
    EXPECT_EQ(out, kNothingBefore);
}

TEST_F(DListTest, DeleteTailRemovesLast) {
    string out = capture_output([&] { abc.dl_delete_tail(); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b"}));

    out = capture_output([&] { abc.dl_delete_after("b"); });
    EXPECT_EQ(out, kNothingAfter);
}

TEST_F(DListTest, DeleteBeforeHeadPrintsNothingBefore) {
    string out = capture_output([&] { abc.dl_delete_before("a"); });
    EXPECT_EQ(out, kNothingBefore);
    EXPECT_EQ(abc.dl_items().size(), 3u);
}

TEST_F(DListTest, DeleteBeforeWalksBackwardThroughPrevLinks) {
    DList l;
    for (const char* s : {"a", "b", "c", "d", "e"}) l.dl_insert_tail(s);

    l.dl_delete_before("e");
    EXPECT_EQ(l.dl_items(), (Strings{"a", "b", "c", "e"}));
    l.dl_delete_before("e");
    l.dl_delete_before("e");
    l.dl_delete_before("e");
    EXPECT_EQ(l.dl_items(), (Strings{"e"}));
    string out = capture_output([&] { l.dl_delete_before("e"); });
    EXPECT_EQ(out, kNothingBefore);
}

TEST_F(DListTest, DeleteAfterHeadAndMiddle) {
    abc.dl_delete_after("a");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "c"}));

    abc.dl_delete_before("c");
    EXPECT_EQ(abc.dl_items(), (Strings{"c"}));
}

TEST_F(DListTest, DeleteAfterTailPrintsNothingAfter) {
    string out = capture_output([&] { abc.dl_delete_after("c"); });
    EXPECT_EQ(out, kNothingAfter);
    EXPECT_EQ(abc.dl_items().size(), 3u);
}

TEST_F(DListTest, DeleteKeyHeadMiddleTail) {
    abc.dl_delete_key("b");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "c"}));
    abc.dl_delete_key("a");
    EXPECT_EQ(abc.dl_items(), (Strings{"c"}));
    string out = capture_output([&] { abc.dl_delete_key("c"); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(abc.dl_items().empty());
}

TEST_F(DListTest, DeleteKeyMissingPrintsNotFound) {
    string out = capture_output([&] { abc.dl_delete_key("zzz"); });
    EXPECT_EQ(out, kNotFound);
    string out2 = capture_output([&] { empty.dl_delete_key("a"); });
    EXPECT_EQ(out2, kNotFound);
}

TEST_F(DListTest, FindReturnsNodeOrNull) {
    EXPECT_TRUE(abc.dl_find("b") != nullptr);
    EXPECT_TRUE(abc.dl_find("zzz") == nullptr);
    EXPECT_TRUE(empty.dl_find("a") == nullptr);
}

TEST_F(DListTest, SearchFoundPrintsAndReturnsKey) {
    string result;
    string out = capture_output([&] { result = abc.dl_search("b"); });
    EXPECT_EQ(result, "b");
    EXPECT_EQ(out, "b\n");
}

TEST_F(DListTest, PrintRecMatchesPrintIter) {
    string rec = capture_output([&] { abc.dl_print_rec(); });
    string iter = capture_output([&] { abc.dl_print_iter(); });
    EXPECT_EQ(rec, iter);
}

TEST_F(DListTest, PrintOfEmptyListSaysEmpty) {
    string iter = capture_output([&] { empty.dl_print_iter(); });
    string rec = capture_output([&] { empty.dl_print_rec(); });
    EXPECT_EQ(iter, "Список пуст\n");
    EXPECT_EQ(rec, "Список пуст\n");
}

TEST(DListLifetimeTest, LargeListIsDestroyedWithoutStackOverflow) {
    DList big;
    for (int i = 0; i < 100000; ++i) big.dl_insert_tail(to_string(i));
    EXPECT_EQ(big.dl_items().size(), 100000u);
}