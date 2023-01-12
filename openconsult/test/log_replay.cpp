#include <sstream>
#include <gtest/gtest.h>
#include "openconsult/src/log_replay.h"

TEST(LogReplayTest, ctor_empty) {
    std::istringstream stream("");
    LogReplay foo(stream);
}

TEST(LogReplayTest, ctor_single_valid_line) {
    std::istringstream stream("R 01\n");
    LogReplay foo(stream);
}

TEST(LogReplayTest, ctor_single_invalid_record_type) {
    std::istringstream stream("G 01\n");
    EXPECT_THROW({
        LogReplay foo(stream);
    }, std::invalid_argument);
}

TEST(LogReplayTest, ctor_single_invalid_record_sep) {
    std::istringstream stream("R_01\n");
    EXPECT_THROW({
        LogReplay foo(stream);
    }, std::invalid_argument);
}

TEST(LogReplayTest, ctor_single_invalid_record_byte) {
    std::istringstream stream("R 011\n");
    EXPECT_THROW({
        LogReplay foo(stream);
    }, std::invalid_argument);
}
