#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/log_replay.h"

using ::testing::ElementsAre;

TEST(LogReplayTest, ctor_empty) {
    std::istringstream stream("");
    LogReplay replay(stream);
}

TEST(LogReplayTest, ctor_single_valid_line) {
    std::istringstream stream("R 01\n");
    LogReplay replay(stream);
}

TEST(LogReplayTest, ctor_single_invalid_record_type) {
    std::istringstream stream("G 01\n");
    EXPECT_THROW({
        LogReplay replay(stream);
    }, std::invalid_argument);
}

TEST(LogReplayTest, ctor_single_invalid_record_sep) {
    std::istringstream stream("R_01\n");
    EXPECT_THROW({
        LogReplay replay(stream);
    }, std::invalid_argument);
}

TEST(LogReplayTest, ctor_single_invalid_record_byte) {
    std::istringstream stream("R 011\n");
    EXPECT_THROW({
        LogReplay replay(stream);
    }, std::invalid_argument);
}

TEST(LogReplayTest, read_single_byte) {
    std::istringstream stream("W 00\nR 01\n");
    LogReplay replay(stream);
    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));
}
