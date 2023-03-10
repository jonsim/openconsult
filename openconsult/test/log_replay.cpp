#include "openconsult/src/log_replay.h"
#include "openconsult/src/common.h"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace openconsult;
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

TEST(LogReplayTest, read_one_byte) {
    std::istringstream stream("R 01\n");
    LogReplay replay(stream);

    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));
}

TEST(LogReplayTest, read_zero_bytes) {
    std::istringstream stream("R 01\n");
    LogReplay replay(stream);

    auto data = replay.read(0);
    EXPECT_THAT(data, ElementsAre());
}

TEST(LogReplayTest, read_one_byte_without_newline) {
    std::istringstream stream("R 01");
    LogReplay replay(stream);

    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));
}

TEST(LogReplayTest, read_too_many_bytes) {
    std::istringstream stream("R 01\n");
    LogReplay replay(stream);

    EXPECT_THROW({
        replay.read(2);
    }, std::runtime_error);
}

TEST(LogReplayTest, read_fewer_than_total_bytes) {
    std::istringstream stream("R 010203\n");
    LogReplay replay(stream);

    auto data = replay.read(2);
    EXPECT_THAT(data, ElementsAre(1u, 2u));
}

TEST(LogReplayTest, read_multiple_times) {
    std::istringstream stream("R 010203\n");
    LogReplay replay(stream);

    auto data1 = replay.read(2);
    EXPECT_THAT(data1, ElementsAre(1u, 2u));

    auto data2 = replay.read(1);
    EXPECT_THAT(data2, ElementsAre(3u));

    EXPECT_THROW({
        replay.read(1);
    }, std::runtime_error);
}

TEST(LogReplayTest, read_across_lines) {
    std::istringstream stream("R 0102\nR 0304\n");
    LogReplay replay(stream);

    auto data1 = replay.read(3);
    EXPECT_THAT(data1, ElementsAre(1u, 2u, 3u));

    auto data2 = replay.read(1);
    EXPECT_THAT(data2, ElementsAre(4u));
}

TEST(LogReplayTest, read_across_skipped_lines) {
    std::istringstream stream("R 0102\nW 1011\nR 0304\n");
    LogReplay replay(stream);

    auto data = replay.read(4);
    EXPECT_THAT(data, ElementsAre(1u, 2u, 3u, 4u));
}

TEST(LogReplayTest, read_initial_skipped_line) {
    std::istringstream stream("W 0102\nR 0304\n");
    LogReplay replay(stream);

    auto data = replay.read(2);
    EXPECT_THAT(data, ElementsAre(3u, 4u));
}

TEST(LogReplayTest, read_no_valid_lines) {
    std::istringstream stream("W 0102\n");
    LogReplay replay(stream);

    EXPECT_THROW({
        replay.read(1);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_one_byte) {
    std::istringstream stream("W 01\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{1u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_zero_bytes) {
    std::istringstream stream("W 01\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes;
    replay.write(bytes);
}

TEST(LogReplayTest, write_fewer_than_total_bytes) {
    std::istringstream stream("W 0102\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{2u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_different_byte) {
    std::istringstream stream("W 01\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{2u}};
    EXPECT_THROW({
        replay.write(bytes);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_too_many_bytes) {
    std::istringstream stream("W 01\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{1u, 1u}};
    EXPECT_THROW({
        replay.write(bytes);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_multiple_times) {
    std::istringstream stream("W 0102\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{1u, 2u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_across_skipped_lines) {
    std::istringstream stream("W 0102\nR 0304\nW 0506\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{5u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_invalid_across_skipped_lines) {
    std::istringstream stream("W 0102\nR 0304\nW 0506\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{3u}};
    EXPECT_THROW({
        replay.write(bytes);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_initial_skipped_line) {
    std::istringstream stream("R 0102\nW 0304\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{3u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_no_valid_lines) {
    std::istringstream stream("R 0102\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{1u}};
    EXPECT_THROW({
        replay.write(bytes);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_differing_from_log) {
    std::istringstream stream("W 0102030405060708\n");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes1 {{1u, 2u}};
    replay.write(bytes1);

    std::vector<uint8_t> bytes2 {{5u, 6u}};
    replay.write(bytes2);

    std::vector<uint8_t> bytes {{3u}};
    EXPECT_THROW({
        replay.write(bytes);
    }, std::runtime_error);
}

TEST(LogReplayTest, read_then_write) {
    std::istringstream stream("R 0102\nW 0304");
    LogReplay replay(stream);

    auto data = replay.read(2);
    EXPECT_THAT(data, ElementsAre(1u, 2u));
    std::vector<uint8_t> bytes {{3u, 4u}};
    replay.write(bytes);
}

TEST(LogReplayTest, write_advances_read_cursor_to_end) {
    std::istringstream stream("R 0102\nW 0304");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{3u}};
    replay.write(bytes);

    EXPECT_THROW({
        replay.read(1);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_advances_read_cursor_to_next) {
    std::istringstream stream("R 0102\nW 0304\nR 0506");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes {{3u, 4u}};
    replay.write(bytes);

    auto data = replay.read(2);
    EXPECT_THAT(data, ElementsAre(5u, 6u));
}

TEST(LogReplayTest, write_resets_read_cursor_to_next) {
    std::istringstream stream("R 0102\nW 0304\nR 0506");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes1 {{3u}};
    replay.write(bytes1);

    auto data1 = replay.read(2);
    EXPECT_THAT(data1, ElementsAre(5u, 6u));

    std::vector<uint8_t> bytes2 {{4u}};
    replay.write(bytes2);

    auto data2 = replay.read(2);
    EXPECT_THAT(data2, ElementsAre(5u, 6u));

    EXPECT_THROW({
        replay.write(bytes1);
    }, std::runtime_error);
    EXPECT_THROW({
        replay.read(1);
    }, std::runtime_error);
}

TEST(LogReplayTest, write_resets_read_cursor_over_skipped_lines) {
    std::istringstream stream("R 0102\nW 03\nR 0506\nW 04\nR 0708");
    LogReplay replay(stream);

    std::vector<uint8_t> bytes1 {{3u, 4u}};
    replay.write(bytes1);

    auto data1 = replay.read(2);
    EXPECT_THAT(data1, ElementsAre(7u, 8u));

    EXPECT_THROW({
        replay.write(bytes1);
    }, std::runtime_error);
    EXPECT_THROW({
        replay.read(1);
    }, std::runtime_error);
}

TEST(LogReplayTest, read_one_byte_wrapped) {
    std::istringstream stream("R 01\n");
    LogReplay replay(stream, true);

    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));

    data = replay.read(3);
    EXPECT_THAT(data, ElementsAre(1u, 1u, 1u));
}

TEST(LogReplayTest, read_multiple_lines_wrapped) {
    std::istringstream stream("R 01\nR 02");
    LogReplay replay(stream, true);

    auto data = replay.read(3);
    EXPECT_THAT(data, ElementsAre(1u, 2u, 1u));

    data = replay.read(3);
    EXPECT_THAT(data, ElementsAre(2u, 1u, 2u));
}

TEST(LogReplayTest, read_write_wrapped) {
    std::istringstream stream("W 01\nR 01\nW 02\nR 02");
    LogReplay replay(stream, true);

    std::vector<uint8_t> bytes1 {{1u}};
    std::vector<uint8_t> bytes2 {{2u}};

    replay.write(bytes1);
    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));

    replay.write(bytes1);
    data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));

    replay.write(bytes2);
    data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(2u));

    replay.write(bytes2);
    data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(2u));
}

TEST(LogReplayTest, write_wrapped_sequence) {
    std::istringstream stream("R 01\nW 02\nR 0304\nW 05");
    LogReplay replay(stream, true);

    std::vector<uint8_t> bytes1 {{2u, 5u}};
    replay.write(bytes1);

    auto data = replay.read(1);
    EXPECT_THAT(data, ElementsAre(1u));

    std::vector<uint8_t> bytes2 {{5u, 2u}};
    replay.write(bytes2);

    data = replay.read(2);
    EXPECT_THAT(data, ElementsAre(3u, 4u));
}
