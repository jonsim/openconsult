#include "openconsult/src/log_recorder.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace openconsult;
using ::testing::ElementsAre;
using ::testing::Exactly;
using ::testing::Return;


class MockByteInterface : public ByteInterface {
public:
    MOCK_METHOD(std::vector<uint8_t>, read, (std::size_t size), (override));
    MOCK_METHOD(void, write, (const std::vector<uint8_t>& bytes), (override));
};

TEST(LogRecorderTest, ctor) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    EXPECT_EQ(stream.str(), "");
}

TEST(LogRecorderTest, single_read) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x1a}));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    recorder.read(1);

    EXPECT_EQ(stream.str(), "R 1a");
}

TEST(LogRecorderTest, single_write) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x1a)))
        .Times(Exactly(1));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes {{0x1a}};
    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "W 1a");
}

TEST(LogRecorderTest, multiple_reads) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x11}));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    recorder.read(1);
    recorder.read(1);

    EXPECT_EQ(stream.str(), "R 1011");
}

TEST(LogRecorderTest, multiple_writes) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x20)))
        .Times(Exactly(1));
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x21)))
        .Times(Exactly(1));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes1 {{0x20}};
    std::vector<uint8_t> bytes2 {{0x21}};
    recorder.write(bytes1);
    recorder.write(bytes2);

    EXPECT_EQ(stream.str(), "W 2021");
}

TEST(LogRecorderTest, read_then_write) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x01}));
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x01)))
        .Times(Exactly(1));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes = recorder.read(1);
    EXPECT_THAT(bytes, ElementsAre(0x01));

    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "R 01\nW 01");
}

TEST(LogRecorderTest, write_then_read) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x01)))
        .Times(Exactly(1));
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x01}));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes {{0x01}};
    recorder.write(bytes);

    bytes = recorder.read(1);
    EXPECT_THAT(bytes, ElementsAre(0x01));

    EXPECT_EQ(stream.str(), "W 01\nR 01");
}

TEST(LogRecorderTest, reads_writes_reads) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(4))
        .WillRepeatedly(Return(std::vector<uint8_t>{0x01, 0x02}));
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x01u, 0x02)))
        .Times(Exactly(2));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes {{0x01, 0x02}};
    recorder.read(2);
    recorder.read(2);
    recorder.write(bytes);
    recorder.write(bytes);
    recorder.read(2);
    recorder.read(2);

    EXPECT_EQ(stream.str(), "R 01020102\nW 01020102\nR 01020102");
}

TEST(LogRecorderTest, writes_reads_writes) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x01u, 0x02)))
        .Times(Exactly(4));
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(2))
        .WillRepeatedly(Return(std::vector<uint8_t>{0x01, 0x02}));

    std::ostringstream stream;
    LogRecorder recorder(std::move(byte_interface), stream);

    std::vector<uint8_t> bytes {{0x01, 0x02}};
    recorder.write(bytes);
    recorder.write(bytes);
    recorder.read(2);
    recorder.read(2);
    recorder.write(bytes);
    recorder.write(bytes);

    EXPECT_EQ(stream.str(), "W 01020102\nR 01020102\nW 01020102");
}

TEST(LogRecorderTest, final_newline) {
    std::ostringstream stream;
    {
        std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
        EXPECT_CALL(*byte_interface, read(1))
            .Times(Exactly(1))
            .WillRepeatedly(Return(std::vector<uint8_t>{0x01}));

        LogRecorder recorder(std::move(byte_interface), stream);
        recorder.read(1);
        EXPECT_EQ(stream.str(), "R 01");
    }

    EXPECT_EQ(stream.str(), "R 01\n");
}
