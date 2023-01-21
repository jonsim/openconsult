#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "openconsult/src/consult_interface.h"

TEST(ECUMetadataTest, toJSON) {
    std::vector<uint8_t> data {0x00, 0x00, 0x04, 0x88, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x05, 0x0F, 0x00};
    ECUMetadata metadata(data);
    EXPECT_EQ("{\n"
              "  \"part_number\": \"0488 23710-50F00\"\n"
              "}", metadata.toJSON());
}


TEST(FaultCodeDataTest, toJSON) {
    std::vector<uint8_t> data {51, 42};
    FaultCodeData code(data);
    EXPECT_EQ("{\n"
              "  \"code\": 51,\n"
              "  \"name\": \"Injector Circuit\",\n"
              "  \"description\": null,\n"
              "  \"starts_since_observed\": 42\n"
              "}", code.toJSON());
}


TEST(FaultCodesTest, toJSON) {
    std::vector<uint8_t> data {34, 13, 45, 17};
    FaultCodes codes(data);
    EXPECT_EQ("[\n"
              "  {\n"
              "    \"code\": 34,\n"
              "    \"name\": \"Knock sensor signal circuit\",\n"
              "    \"description\": \"At least one knock sensor indicates the output voltage of approx. 4V or greater (open circuit) or less than approx. 1V (short circuit).\",\n"
              "    \"starts_since_observed\": 13\n"
              "  },\n"
              "  {\n"
              "    \"code\": 45,\n"
              "    \"name\": \"Injector Leak\",\n"
              "    \"description\": null,\n"
              "    \"starts_since_observed\": 17\n"
              "  }\n"
              "]", codes.toJSON());
}


TEST(EngineParametersTest, toJSON) {
    std::vector<EngineParameter> params {EngineParameter::ENGINE_RPM, EngineParameter::BATTERY_VOLTAGE};
    std::vector<uint8_t> data {0x01, 0x59, 0x97};
    EngineParameters parameters(params, data);
    EXPECT_EQ("{\n"
              "  \"engine_speed_rpm\": 4312.50,\n"
              "  \"battery_v\": 12.08\n"
              "}", parameters.toJSON());
}


class MockByteInterface : public ByteInterface {
public:
    MOCK_METHOD(std::vector<uint8_t>, read, (std::size_t size), (override));
    MOCK_METHOD(void, write, (const std::vector<uint8_t>& bytes), (override));
};

using ::testing::AtLeast;
using ::testing::ElementsAre;
using ::testing::Exactly;
using ::testing::Return;

TEST(ConsultInterfaceTest, ctor) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
}

TEST(ConsultInterfaceTest, ctor_non_empty_buffer) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(3))
        .WillOnce(Return(std::vector<uint8_t>{0x00}))
        .WillOnce(Return(std::vector<uint8_t>{0x00}))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
}

TEST(ConsultInterfaceTest, readECUMetadata) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(3))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x2F}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xD0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x16}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(22))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x00, 0x21, 0x14, 0x80, 0x20, 0x00, 0x00, 0x3F,
                                              0x80, 0x80, 0xE2, 0x20, 0x00, 0x00, 0x28, 0xFF,
                                              0xFF, 0x41, 0x41, 0x35, 0x30, 0x32}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    auto ecu = iface.readECUMetadata();

    EXPECT_EQ("1480 23710-353032", ecu.part_number);
}

TEST(ConsultInterfaceTest, readECUMetadata_invalid_response) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x2E}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xD0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    EXPECT_THROW({
        iface.readECUMetadata();
    }, std::runtime_error);
}

TEST(ConsultInterfaceTest, readFaultCodes_single) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(3))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x2E}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xD1)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x02}))
        .WillOnce(Return(std::vector<uint8_t>{0x33, 0x0B}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    auto codes = iface.readFaultCodes();

    EXPECT_EQ(1, codes.fault_codes.size());
    EXPECT_EQ(FaultCode::FUEL_INJECTOR, codes.fault_codes[0].fault_code);
    EXPECT_EQ(11, codes.fault_codes[0].starts_since_observed);
}

TEST(ConsultInterfaceTest, readFaultCodes_double) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(3))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x2E}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xD1)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x04}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(4))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x33, 0x0B, 0x69, 0x42}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    auto codes = iface.readFaultCodes();

    EXPECT_EQ(2, codes.fault_codes.size());
    EXPECT_EQ(FaultCode::FUEL_INJECTOR, codes.fault_codes[0].fault_code);
    EXPECT_EQ(11, codes.fault_codes[0].starts_since_observed);
    EXPECT_EQ(FaultCode::EXHAUST_GAS_RECIRCULATION_VALVE, codes.fault_codes[1].fault_code);
    EXPECT_EQ(66, codes.fault_codes[1].starts_since_observed);
}

TEST(ConsultInterfaceTest, readFaultCodes_invalid_response) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0x2F}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xD1)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    EXPECT_THROW({
        iface.readFaultCodes();
    }, std::runtime_error);
}

TEST(ConsultInterfaceTest, readEngineParameters_single) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(3))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0xB4}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x5A, 0x0C)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0xA5, 0x0C}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x01}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    std::vector<EngineParameter> params {EngineParameter::BATTERY_VOLTAGE};
    auto data = iface.readEngineParameters(params);

    EXPECT_EQ(1, data.parameters.size());
    EXPECT_EQ(data.parameters[EngineParameter::BATTERY_VOLTAGE], 14.4);
}

TEST(ConsultInterfaceTest, readEngineParameters_multiple) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x5A, 0x00, 0x5A, 0x01,
                                                   0x5A, 0x0B, 0x5A, 0x0C)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(8))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xA5, 0x00, 0xA5, 0x01,
                                              0xA5, 0x0B, 0xA5, 0x0C}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x04}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(4))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x00, 0x75, 0x00, 0xB4}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    std::vector<EngineParameter> params {EngineParameter::ENGINE_RPM,
                                         EngineParameter::VEHICLE_SPEED,
                                         EngineParameter::BATTERY_VOLTAGE};
    auto data = iface.readEngineParameters(params);

    EXPECT_EQ(3, data.parameters.size());
    EXPECT_EQ(data.parameters[EngineParameter::ENGINE_RPM], 1462.5);
    EXPECT_EQ(data.parameters[EngineParameter::VEHICLE_SPEED], 0.0);
    EXPECT_EQ(data.parameters[EngineParameter::BATTERY_VOLTAGE], 14.4);
}

TEST(ConsultInterfaceTest, readEngineParameters_invalid_response) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x5A, 0x0C)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xA5, 0x0D}))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    std::vector<EngineParameter> params {EngineParameter::BATTERY_VOLTAGE};
    EXPECT_THROW({
        iface.readEngineParameters(params);
    }, std::runtime_error);
}

TEST(ConsultInterfaceTest, streamEngineParameters_single) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(5))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0xB4}))
        .WillOnce(Return(std::vector<uint8_t>{0xB5}))
        .WillOnce(Return(std::vector<uint8_t>{0xB6}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x5A, 0x0C)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(4))
        .WillOnce(Return(std::vector<uint8_t>{0xA5, 0x0C}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x01}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x01}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x01}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    std::vector<EngineParameter> params {EngineParameter::BATTERY_VOLTAGE};
    {
        auto stream = iface.streamEngineParameters(params);

        auto data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::BATTERY_VOLTAGE], 14.40);

        data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::BATTERY_VOLTAGE], 14.48);

        data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::BATTERY_VOLTAGE], 14.56);
    }
}

TEST(ConsultInterfaceTest, streamEngineParameters_multiple) {
    std::unique_ptr<MockByteInterface> byte_interface(new MockByteInterface);
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xFF, 0xFF, 0xEF)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(1))
        .Times(Exactly(2))
        .WillOnce(Return(std::vector<uint8_t>{0x10}))
        .WillOnce(Return(std::vector<uint8_t>{0xCF}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x5A, 0x00, 0x5A, 0x01)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(4))
        .Times(Exactly(1))
        .WillOnce(Return(std::vector<uint8_t>{0xA5, 0x00, 0xA5, 0x01}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0xF0)))
        .Times(Exactly(1))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, read(2))
        .Times(Exactly(6))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x02}))
        .WillOnce(Return(std::vector<uint8_t>{0x00, 0x75}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x02}))
        .WillOnce(Return(std::vector<uint8_t>{0x00, 0x85}))
        .WillOnce(Return(std::vector<uint8_t>{0xFF, 0x02}))
        .WillOnce(Return(std::vector<uint8_t>{0x00, 0x95}))
        .RetiresOnSaturation();
    EXPECT_CALL(*byte_interface, write(ElementsAre(0x30)))
        .Times(Exactly(1))
        .RetiresOnSaturation();

    ConsultInterface iface(std::move(byte_interface));
    std::vector<EngineParameter> params {EngineParameter::ENGINE_RPM};
    {
        auto stream = iface.streamEngineParameters(params);

        auto data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::ENGINE_RPM], 1462.5);

        data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::ENGINE_RPM], 1662.5);

        data = stream.getFrame();
        EXPECT_EQ(1, data.parameters.size());
        EXPECT_EQ(data.parameters[EngineParameter::ENGINE_RPM], 1862.5);
    }
}
