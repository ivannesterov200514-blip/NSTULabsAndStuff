#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "capture.h"
#include "stack.h"

using namespace std;

using Strings = vector<string>;

TEST_CASE("Stack: new stack is empty", "[stack]") {
    Stack s;
    REQUIRE(s.st_items().empty());
}

TEST_CASE("Stack: push puts the newest element on top", "[stack]") {
    Stack s;
    s.st_push("a");
    s.st_push("b");
    s.st_push("c");
    REQUIRE(s.st_items() == Strings{"c", "b", "a"});
}

TEST_CASE("Stack: pop on empty stack prints an error", "[stack]") {
    Stack s;
    string out = capture_output([&] { s.st_pop(); });
    REQUIRE(out == "Ошибка: Стек пуст\n");
    REQUIRE(s.st_items().empty());
}

TEST_CASE("Stack: elements come out in LIFO order", "[stack]") {
    Stack s;
    string expected;
    for (int i = 0; i < 1000; ++i) s.st_push(to_string(i));
    for (int i = 999; i >= 0; --i) expected += to_string(i) + "\n";
    string out = capture_output([&] {
        for (int i = 0; i < 1000; ++i) s.st_pop();
    });
    REQUIRE(out == expected);
    REQUIRE(s.st_items().empty());
}

TEST_CASE("Stack: print format", "[stack]") {
    SECTION("empty") {
        Stack s;
        string out = capture_output([&] { s.st_print(); });
        REQUIRE(out == "Стек пуст\n");
    }
    SECTION("with elements the top is printed first") {
        Stack s;
        s.st_push("a");
        s.st_push("b");
        s.st_push("c");
        string out = capture_output([&] { s.st_print(); });
        REQUIRE(out == "c\nb\na\nnullptr\n");
    }
}

TEST_CASE("Stack: large stack is destroyed without stack overflow", "[stack]") {
    Stack s;
    for (int i = 0; i < 100000; ++i) s.st_push(to_string(i));
    REQUIRE(s.st_items().size() == 100000);
}