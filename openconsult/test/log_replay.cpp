#include <gtest/gtest.h>
#include "openconsult/src/log_replay.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(LogReplayTest, Basic) {
    LogReplay foo("boogle.txt");
    EXPECT_EQ(NULL, foo);
}