#include <gtest/gtest.h>
#include "taskmanager.h"

TEST(TaskManagerTest, RejectInvalidTransitions) {
    TaskManager manager;
    uint64_t id = manager.createTask("calculate", "2 + 2", 80);

    EXPECT_THROW(manager.completeTask(id), std::logic_error);
    EXPECT_THROW(manager.failTask(id, "error"), std::logic_error);

    EXPECT_EQ(manager.getTask(id)->status, "queued");

    manager.startNextTask();

    EXPECT_THROW(manager.cancelTask(id), std::logic_error);
    EXPECT_EQ(manager.getTask(id)->status, "running");
}

