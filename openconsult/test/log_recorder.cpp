#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/log_recorder.h"

TEST(LogRecorderTest, ctor) {
    std::ostringstream stream;
    LogRecorder recorder(stream);

    EXPECT_EQ(stream.str(), "");
}

TEST(LogRecorderTest, single_read) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{26u}};
    recorder.read(bytes);

    EXPECT_EQ(stream.str(), "R 1a");
}

TEST(LogRecorderTest, single_write) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{26u}};
    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "W 1a");
}

TEST(LogRecorderTest, multiple_reads) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes1 {{16u}};
    std::vector<uint8_t> bytes2 {{17u}};
    recorder.read(bytes1);
    recorder.read(bytes2);

    EXPECT_EQ(stream.str(), "R 1011");
}

TEST(LogRecorderTest, multiple_writes) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes1 {{32u}};
    std::vector<uint8_t> bytes2 {{33u}};
    recorder.write(bytes1);
    recorder.write(bytes2);

    EXPECT_EQ(stream.str(), "W 2021");
}

TEST(LogRecorderTest, read_then_write) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{1u}};
    recorder.read(bytes);
    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "R 01\nW 01");
}

TEST(LogRecorderTest, write_then_read) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{1u}};
    recorder.write(bytes);
    recorder.read(bytes);

    EXPECT_EQ(stream.str(), "W 01\nR 01");
}

TEST(LogRecorderTest, reads_writes_reads) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{1u, 2u}};
    recorder.read(bytes);
    recorder.read(bytes);
    recorder.write(bytes);
    recorder.write(bytes);
    recorder.read(bytes);
    recorder.read(bytes);

    EXPECT_EQ(stream.str(), "R 01020102\nW 01020102\nR 01020102");
}

TEST(LogRecorderTest, writes_reads_writes) {
    std::ostringstream stream;
    LogRecorder recorder(stream);
    std::vector<uint8_t> bytes {{1u, 2u}};
    recorder.write(bytes);
    recorder.write(bytes);
    recorder.read(bytes);
    recorder.read(bytes);
    recorder.write(bytes);
    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "W 01020102\nR 01020102\nW 01020102");
}

TEST(LogRecorderTest, final_newline) {
    std::ostringstream stream;
    {
        LogRecorder recorder(stream);
        std::vector<uint8_t> bytes {{1u}};
        recorder.read(bytes);
        EXPECT_EQ(stream.str(), "R 01");
    }

    EXPECT_EQ(stream.str(), "R 01\n");
}
