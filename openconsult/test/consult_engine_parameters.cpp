#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/consult_engine_parameters.h"

using ::testing::ElementsAre;

TEST(ConsultEngineParametersTest, engineParameterCommand) {
    EXPECT_THAT(engineParameterCommand(EngineParameter::ENGINE_RPM),
                ElementsAre(0x00, 0x01));
    EXPECT_THAT(engineParameterCommand(EngineParameter::BATTERY_VOLTAGE),
                ElementsAre(0x0c));
    EXPECT_THROW({
        engineParameterCommand(static_cast<EngineParameter>(0xffu));
    }, std::invalid_argument);
}


TEST(ConsultEngineParametersTest, engineParameterDecode_one_byte_valid) {
    EXPECT_EQ(engineParameterDecode(EngineParameter::COOLANT_TEMPERATURE, std::vector<uint8_t>{{0x25}}),
              -13.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_O2_SENSOR_VOLTAGE, std::vector<uint8_t>{{0x99}}),
              1.53);
    EXPECT_EQ(engineParameterDecode(EngineParameter::VEHICLE_SPEED, std::vector<uint8_t>{{0x1c}}),
              56.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::BATTERY_VOLTAGE, std::vector<uint8_t>{{0x97}}),
              12.08);
    EXPECT_EQ(engineParameterDecode(EngineParameter::THROTTLE_POSITION, std::vector<uint8_t>{{0x99}}),
              3.06);
    EXPECT_EQ(engineParameterDecode(EngineParameter::IGNITION_TIMING, std::vector<uint8_t>{{0x73}}),
              -5.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::AAC_VALVE, std::vector<uint8_t>{{0x75}}),
              58.5);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_AIR_FUEL_ALPHA, std::vector<uint8_t>{{0x40}}),
              64.0);
}

TEST(ConsultEngineParametersTest, engineParameterDecode_two_byte_valid) {
    EXPECT_EQ(engineParameterDecode(EngineParameter::ENGINE_RPM, std::vector<uint8_t>{{0x01, 0x59}}),
              4312.5);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_MAF_VOLTAGE, std::vector<uint8_t>{{0x02, 0x69}}),
              3.085);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_INJECTION_TIMING, std::vector<uint8_t>{{0x11, 0xa2}}),
              0.04514);
}

TEST(ConsultEngineParametersTest, engineParameterDecode_one_byte_invalid) {
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::BATTERY_VOLTAGE, std::vector<uint8_t>{});
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::BATTERY_VOLTAGE, std::vector<uint8_t>{{0x01, 0x02}});
    }, std::invalid_argument);
}

TEST(ConsultEngineParametersTest, engineParameterDecode_two_byte_invalid) {
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::ENGINE_RPM, std::vector<uint8_t>{});
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::ENGINE_RPM, std::vector<uint8_t>{{0x01}});
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::ENGINE_RPM, std::vector<uint8_t>{{0x01, 0x02, 0x03}});
    }, std::invalid_argument);
}

TEST(ConsultEngineParametersTest, engineParameterDecode_parameter_invalid) {
    EXPECT_THROW({
        engineParameterDecode(static_cast<EngineParameter>(0xffu), std::vector<uint8_t>{});
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(static_cast<EngineParameter>(0xffu), std::vector<uint8_t>{{0x01}});
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(static_cast<EngineParameter>(0xffu), std::vector<uint8_t>{{0x01, 0x02}});
    }, std::invalid_argument);
}


TEST(ConsultEngineParametersTest, engineParameterId) {
    EXPECT_EQ(engineParameterId(EngineParameter::ENGINE_RPM),
              "engine_speed_rpm");
    EXPECT_EQ(engineParameterId(EngineParameter::BATTERY_VOLTAGE),
              "battery_v");
    EXPECT_THROW({
        engineParameterId(static_cast<EngineParameter>(0xffu));
    }, std::invalid_argument);
}


TEST(ConsultEngineParametersTest, engineParameterName) {
    EXPECT_EQ(engineParameterName(EngineParameter::ENGINE_RPM),
              "Engine speed (RPM)");
    EXPECT_EQ(engineParameterName(EngineParameter::BATTERY_VOLTAGE),
              "Battery voltage (V)");
    EXPECT_THROW({
        engineParameterName(static_cast<EngineParameter>(0xffu));
    }, std::invalid_argument);
}


TEST(ConsultEngineParametersTest, engineParameterDescription) {
    EXPECT_EQ(engineParameterDescription(EngineParameter::BATTERY_VOLTAGE),
              "The power supply voltage of the ECM.");
    EXPECT_EQ(engineParameterDescription(EngineParameter::TURBO_BOOST_SENSOR),
              "");
    EXPECT_THROW({
        engineParameterDescription(static_cast<EngineParameter>(0xffu));
    }, std::invalid_argument);
}
