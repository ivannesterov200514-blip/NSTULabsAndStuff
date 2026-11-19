#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "capture.h"
#include "queue.h"

using namespace std;

using Strings = vector<string>;

TEST_CASE("Queue: new queue is empty", "[queue]") {
    Queue q;
    REQUIRE(q.q_items().empty());
}

TEST_CASE("Queue: push appends to the tail", "[queue]") {
    Queue q;
    q.q_push("a");
    q.q_push("b");
    q.q_push("c");
    REQUIRE(q.q_items() == Strings{"a", "b", "c"});
}

TEST_CASE("Queue: pop on empty queue prints an error", "[queue]") {
    Queue q;
    string out = capture_output([&] { q.q_pop(); });
    REQUIRE(out == "Ошибка: Очередь пуста\n");
}

TEST_CASE("Queue: elements come out in FIFO order", "[queue]") {
    Queue q;
    string expected;
    for (int i = 0; i < 1000; ++i) {
        q.q_push(to_string(i));
        expected += to_string(i) + "\n";
    }
    string out = capture_output([&] {
        for (int i = 0; i < 1000; ++i) q.q_pop();
    });
    REQUIRE(out == expected);
    REQUIRE(q.q_items().empty());
}

TEST_CASE("Queue: tail is reset when the queue becomes empty", "[queue]") {
    Queue q;
    q.q_push("a");
    capture_output([&] { q.q_pop(); });

    q.q_push("b");
    q.q_push("c");
    REQUIRE(q.q_items() == Strings{"b", "c"});
}

TEST_CASE("Queue: print format", "[queue]") {
    SECTION("empty") {
        Queue q;
        string out = capture_output([&] { q.q_print(); });
        REQUIRE(out == "Очередь пуста\n");
    }
    SECTION("with elements the head is printed first") {
        Queue q;
        q.q_push("a");
        q.q_push("b");
        string out = capture_output([&] { q.q_print(); });
        REQUIRE(out == "Head: a -> b -> : Tail\n");
    }
}
