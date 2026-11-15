// Google Test: двусвязный список DList.
//
// У DList нет метода обхода "с конца", поэтому связи prev проверяются косвенно:
// dl_insert_before / dl_delete_before / dl_delete_tail работают именно через prev и tail.
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "capture.h"
#include "dlist.h"

using std::string;
using std::vector;
using Strings = vector<string>;

namespace {

const string kNotFound = "Ошибка: Не найдено\n";
const string kEmptyList = "Ошибка: Список пуст\n";
const string kLastRemoved = "Последний узел списка удален.\n";
const string kNothingBefore = "Ошибка: Перед элементом ничего нет\n";
const string kNothingAfter = "Ошибка: После элемента ничего нет\n";

// Фикстура: пустой список и список a <-> b <-> c.
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

}  // namespace

// ---------- создание и вставка ----------

TEST_F(DListTest, NewListIsEmpty) {
    EXPECT_TRUE(empty.dl_items().empty());
}

TEST_F(DListTest, InsertTailKeepsOrder) {
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "c"}));
}

TEST_F(DListTest, InsertHeadPutsElementFirst) {
    abc.dl_insert_head("x");
    EXPECT_EQ(abc.dl_items(), (Strings{"x", "a", "b", "c"}));
}

TEST_F(DListTest, InsertHeadOnEmptyListSetsTail) {
    empty.dl_insert_head("x");
    EXPECT_EQ(empty.dl_items(), (Strings{"x"}));
    // если хвост не выставлен, dl_delete_tail сломается
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

TEST_F(DListTest, InsertAfterMissingKeyPrintsError) {
    string out = capture_output([&] { abc.dl_insert_after("zzz", "x"); });
    EXPECT_EQ(out, kNotFound);
    EXPECT_EQ(abc.dl_items().size(), 3u);
}

TEST_F(DListTest, InsertAfterTailUpdatesTail) {
    abc.dl_insert_after("c", "x");
    // новый хвост должен быть найден через tail
    abc.dl_delete_tail();
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "c"}));
}

TEST_F(DListTest, InsertBeforeHeadUpdatesPrevOfOldHead) {
    abc.dl_insert_before("a", "x");
    // prev старой головы должен указывать на новый узел
    abc.dl_delete_before("a");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "c"}));
}

// ---------- удаление с головы и с хвоста ----------

TEST_F(DListTest, DeleteHeadRemovesFirst) {
    string out = capture_output([&] { abc.dl_delete_head(); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(abc.dl_items(), (Strings{"b", "c"}));
    // у новой головы prev должен быть пустым
    out = capture_output([&] { abc.dl_delete_before("b"); });
    EXPECT_EQ(out, kNothingBefore);
}

TEST_F(DListTest, DeleteHeadOnEmptyListPrintsError) {
    string out = capture_output([&] { empty.dl_delete_head(); });
    EXPECT_EQ(out, kEmptyList);
}

TEST_F(DListTest, DeleteHeadOfSingleElementReportsLastNode) {
    empty.dl_insert_tail("x");
    string out = capture_output([&] { empty.dl_delete_head(); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(empty.dl_items().empty());
}

TEST_F(DListTest, DeleteTailRemovesLast) {
    string out = capture_output([&] { abc.dl_delete_tail(); });
    EXPECT_EQ(out, "");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b"}));
    // у нового хвоста next должен быть пустым
    out = capture_output([&] { abc.dl_delete_after("b"); });
    EXPECT_EQ(out, kNothingAfter);
}

TEST_F(DListTest, DeleteTailOnEmptyListPrintsError) {
    string out = capture_output([&] { empty.dl_delete_tail(); });
    EXPECT_EQ(out, kEmptyList);
}

TEST_F(DListTest, DeleteTailOfSingleElementReportsLastNode) {
    empty.dl_insert_head("x");
    string out = capture_output([&] { empty.dl_delete_tail(); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(empty.dl_items().empty());
    // после опустошения список остается рабочим
    empty.dl_insert_tail("y");
    EXPECT_EQ(empty.dl_items(), (Strings{"y"}));
}

// ---------- удаление перед / после ключа ----------

TEST_F(DListTest, DeleteBeforeHeadPrintsNothingBefore) {
    string out = capture_output([&] { abc.dl_delete_before("a"); });
    EXPECT_EQ(out, kNothingBefore);
    EXPECT_EQ(abc.dl_items().size(), 3u);
}

TEST_F(DListTest, DeleteBeforeSecondRemovesHead) {
    abc.dl_delete_before("b");
    EXPECT_EQ(abc.dl_items(), (Strings{"b", "c"}));
}

TEST_F(DListTest, DeleteBeforeTailRemovesMiddle) {
    abc.dl_delete_before("c");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "c"}));
}

TEST_F(DListTest, DeleteBeforeMissingKeyPrintsNotFound) {
    string out = capture_output([&] { abc.dl_delete_before("zzz"); });
    EXPECT_EQ(out, kNotFound);
}

TEST_F(DListTest, DeleteBeforeWalksBackwardThroughPrevLinks) {
    DList l;
    for (const char* s : {"a", "b", "c", "d", "e"}) l.dl_insert_tail(s);
    // каждый вызов удаляет предшественника "e" и опирается на корректность prev
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
    // prev у "c" теперь должен указывать на "a"
    abc.dl_delete_before("c");
    EXPECT_EQ(abc.dl_items(), (Strings{"c"}));
}

TEST_F(DListTest, DeleteAfterTailPrintsNothingAfter) {
    string out = capture_output([&] { abc.dl_delete_after("c"); });
    EXPECT_EQ(out, kNothingAfter);
    EXPECT_EQ(abc.dl_items().size(), 3u);
}

TEST_F(DListTest, DeleteAfterMissingKeyPrintsNotFound) {
    string out = capture_output([&] { abc.dl_delete_after("zzz"); });
    EXPECT_EQ(out, kNotFound);
}

// ---------- удаление по ключу ----------

TEST_F(DListTest, DeleteKeyHeadMiddleTail) {
    abc.dl_delete_key("b");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "c"}));
    abc.dl_delete_key("a");
    EXPECT_EQ(abc.dl_items(), (Strings{"c"}));
    string out = capture_output([&] { abc.dl_delete_key("c"); });
    EXPECT_EQ(out, kLastRemoved);
    EXPECT_TRUE(abc.dl_items().empty());
}

TEST_F(DListTest, DeleteKeyMiddleKeepsLinksConsistent) {
    abc.dl_delete_key("b");
    // a <-> c: prev у "c" - это "a", next у "a" - это "c"
    abc.dl_delete_before("c");
    EXPECT_EQ(abc.dl_items(), (Strings{"c"}));
}

TEST_F(DListTest, DeleteKeyTailUpdatesTail) {
    abc.dl_delete_key("c");
    abc.dl_insert_tail("d");
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b", "d"}));
    abc.dl_delete_tail();
    EXPECT_EQ(abc.dl_items(), (Strings{"a", "b"}));
}

TEST_F(DListTest, DeleteKeyMissingPrintsNotFound) {
    string out = capture_output([&] { abc.dl_delete_key("zzz"); });
    EXPECT_EQ(out, kNotFound);
    string out2 = capture_output([&] { empty.dl_delete_key("a"); });
    EXPECT_EQ(out2, kNotFound);
}

// ---------- поиск ----------

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

TEST_F(DListTest, SearchMissingReturnsEmptyString) {
    string result = "unchanged";
    string out = capture_output([&] { result = abc.dl_search("zzz"); });
    EXPECT_EQ(result, "");
    EXPECT_EQ(out, kNotFound);
}

// ---------- печать ----------

TEST_F(DListTest, PrintIterShowsBothEnds) {
    string out = capture_output([&] { abc.dl_print_iter(); });
    EXPECT_EQ(out, "nullptr <-> a <-> b <-> c <-> nullptr\n");
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

// ---------- жизненный цикл ----------

TEST(DListLifetimeTest, LargeListIsDestroyedWithoutStackOverflow) {
    DList big;
    for (int i = 0; i < 100000; ++i) big.dl_insert_tail(std::to_string(i));
    EXPECT_EQ(big.dl_items().size(), 100000u);
}
