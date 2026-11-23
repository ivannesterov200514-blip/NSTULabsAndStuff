#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include "taskmanager.h"

TEST(TaskManagerTest, CreateAndGetTask) {
    TaskManager manager;

    uint64_t id1 = manager.createTask("calculate", "2 + 2", 80);
    uint64_t id2 = manager.createTask("download", "file", 30);

    EXPECT_EQ(id1, 1);
    EXPECT_EQ(id2, 2);
    EXPECT_EQ(manager.getTask(id1)->type, "calculate");
    EXPECT_EQ(manager.getTask(id1)->payload, "2 + 2");
    EXPECT_EQ(manager.getTask(id1)->priority, 80);
    EXPECT_EQ(manager.getTask(id1)->status, "queued");
    EXPECT_FALSE(manager.getTask(id1)->created_at.empty());
    EXPECT_EQ(manager.getTask(id1)->created_at, manager.getTask(id1)->updated_at);
}

TEST(TaskManagerTest, CompleteRunningTask) {
    TaskManager manager;
    uint64_t id = manager.createTask("calculate", "2 + 2", 80);

    EXPECT_EQ(manager.startNextTask()->id, id);
    EXPECT_EQ(manager.getTask(id)->status, "running");

    manager.completeTask(id);
    EXPECT_EQ(manager.getTask(id)->status, "completed");
}

TEST(TaskManagerTest, FailRunningTask) {
    TaskManager manager;
    uint64_t id = manager.createTask("calculate", "bad input", 80);

    manager.startNextTask();
    manager.failTask(id, "Ошибка вычисления");

    EXPECT_EQ(manager.getTask(id)->status, "failed");
    EXPECT_EQ(manager.getTask(id)->error, "Ошибка вычисления");
}

TEST(TaskManagerTest, RejectInvalidTransitions) {
    TaskManager manager;
    uint64_t id = manager.createTask("calculate", "2 + 2", 80);

    EXPECT_THROW(manager.completeTask(id), std::logic_error);
    EXPECT_THROW(manager.failTask(id, "error"), std::logic_error);
    EXPECT_EQ(manager.getTask(id)->status, "queued");

    manager.startNextTask();
    EXPECT_THROW(manager.cancelTask(id), std::logic_error);
    EXPECT_EQ(manager.getTask(id)->status, "running");

    manager.completeTask(id);
    EXPECT_THROW(manager.completeTask(id), std::logic_error);
    EXPECT_THROW(manager.failTask(id, "error"), std::logic_error);
    EXPECT_THROW(manager.cancelTask(id), std::logic_error);
    EXPECT_EQ(manager.getTask(id)->status, "completed");
}

TEST(TaskManagerTest, CancelledTaskIsSkipped) {
    TaskManager manager;
    uint64_t cancelledId = manager.createTask("first", "data", 100);
    uint64_t nextId = manager.createTask("second", "data", 50);

    manager.cancelTask(cancelledId);

    EXPECT_EQ(manager.startNextTask()->id, nextId);
    EXPECT_EQ(manager.getTask(cancelledId)->status, "cancelled");
    EXPECT_EQ(manager.getTask(nextId)->status, "running");
}

TEST(TaskManagerTest, EmptyQueue) {
    TaskManager manager;
    EXPECT_THROW(manager.startNextTask(), std::out_of_range);
}

TEST(TaskManagerTest, OnlyCancelledTasks) {
    TaskManager manager;
    uint64_t id = manager.createTask("calculate", "2 + 2", 100);

    manager.cancelTask(id);
    EXPECT_THROW(manager.startNextTask(), std::out_of_range);
    EXPECT_EQ(manager.getTask(id)->status, "cancelled");
}

TEST(TaskManagerTest, HigherPriorityRunsFirst) {
    TaskManager manager;
    uint64_t low = manager.createTask("low", "data", 10);
    uint64_t high = manager.createTask("high", "data", 90);
    uint64_t middle = manager.createTask("middle", "data", 50);

    EXPECT_EQ(manager.startNextTask()->id, high);
    EXPECT_EQ(manager.startNextTask()->id, middle);
    EXPECT_EQ(manager.startNextTask()->id, low);
}

TEST(TaskManagerTest, EqualPriorityUsesCreationOrder) {
    TaskManager manager;
    uint64_t first = manager.createTask("first", "data", 70);
    uint64_t second = manager.createTask("second", "data", 70);
    uint64_t third = manager.createTask("third", "data", 70);

    EXPECT_EQ(manager.startNextTask()->id, first);
    EXPECT_EQ(manager.startNextTask()->id, second);
    EXPECT_EQ(manager.startNextTask()->id, third);
}

TEST(TaskManagerTest, InvalidPriorityDoesNotCreateTask) {
    TaskManager manager;

    EXPECT_THROW(manager.createTask("calculate", "data", 101), std::invalid_argument);
    EXPECT_EQ(manager.getStats().total, 0);

    uint64_t id = manager.createTask("calculate", "data", 100);
    EXPECT_EQ(id, 1ULL);
    EXPECT_THROW(manager.getTask(999), std::out_of_range);
}

TEST(TaskManagerTest, StatsAndIndexAfterChanges) {
    TaskManager manager;
    uint64_t completed = manager.createTask("a", "data", 100);
    uint64_t failed = manager.createTask("b", "data", 90);
    uint64_t cancelled = manager.createTask("c", "data", 80);
    uint64_t queued1 = manager.createTask("d", "data", 70);
    uint64_t queued2 = manager.createTask("e", "data", 60);

    EXPECT_EQ(manager.startNextTask()->id, completed);
    manager.completeTask(completed);
    EXPECT_EQ(manager.startNextTask()->id, failed);
    manager.failTask(failed, "error");
    manager.cancelTask(cancelled);

    TaskStats stats = manager.getStats();
    EXPECT_EQ(stats.total, 5);
    EXPECT_EQ(stats.queued, 2);
    EXPECT_EQ(stats.running, 0);
    EXPECT_EQ(stats.completed, 1);
    EXPECT_EQ(stats.failed, 1);
    EXPECT_EQ(stats.cancelled, 1);
    EXPECT_EQ(stats.queue_size, 2);
    EXPECT_EQ(stats.max_priority, 70);

    EXPECT_EQ(manager.getTask(completed)->status, "completed");
    EXPECT_EQ(manager.getTask(failed)->status, "failed");
    EXPECT_EQ(manager.getTask(cancelled)->status, "cancelled");
    EXPECT_EQ(manager.getTask(queued1)->status, "queued");
    EXPECT_EQ(manager.getTask(queued2)->status, "queued");

    Arr result;
    manager.listTasks(result, "queued");
    ASSERT_EQ(result.arr_getsz(), 2);
    EXPECT_EQ(result.arr_search(0)->id, queued1);
    EXPECT_EQ(result.arr_search(1)->id, queued2);

    EXPECT_EQ(manager.startNextTask()->id, queued1);
    EXPECT_EQ(manager.startNextTask()->id, queued2);
    EXPECT_THROW(manager.startNextTask(), std::out_of_range);
}


TEST(TaskManagerTest, SaveAndLoad) {
    TaskManager manager;

    uint64_t id1 = manager.createTask("calculate", "2 + 2", 80);
    uint64_t id2 = manager.createTask("download", "file.txt", 50);

    manager.startNextTask();
    manager.completeTask(id1);
    manager.saveToFile("test_tasks.dat");
    TaskManager loaded;
    loaded.loadFromFile("test_tasks.dat");

    EXPECT_EQ(loaded.getTask(id1)->status, "completed");
    EXPECT_EQ(loaded.getTask(id2)->status, "queued");
    EXPECT_EQ(loaded.getTask(id1)->payload, "2 + 2");
    EXPECT_EQ(loaded.getTask(id2)->priority, 50);
    EXPECT_EQ(loaded.getStats().total, 2);

    std::filesystem::remove("test_tasks.dat");
}


TEST(TaskManagerTest, CorruptedFile) {
    const string path = "test_corrupted.dat";

    {
        ofstream file(path);

        file << "TASKS_V1\n";
        file << "3 2\n";
        file << "2\n";
        file << "1 80 \"calculate\" \"2 + 2\" queued \"2026-11-23 14:30:00\" \"2026-11-23 14:30:00\" \"\"\n";
        file << "2 50 \"download\"\n";
    }

    TaskManager manager;
    uint64_t id = manager.createTask("original", "test", 60);

    EXPECT_THROW(manager.loadFromFile(path), runtime_error);

    EXPECT_EQ(manager.getStats().total, 1);
    EXPECT_EQ(manager.getTask(id)->type, "original");
    EXPECT_EQ(manager.getTask(id)->payload, "test");
    EXPECT_EQ(manager.getTask(id)->status, "queued");
    EXPECT_EQ(manager.createTask("next", "test", 50), 2);
    EXPECT_TRUE(filesystem::exists(path));

    filesystem::remove(path);
}