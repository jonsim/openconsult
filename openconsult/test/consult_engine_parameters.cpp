#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/consult_engine_parameters.h"

using ::testing::ElementsAre;

TEST(ConsultEngineParametersTest, engineParameterCommand) {
    EXPECT_THAT(engineParameterCommand(EngineParameter::ENGINE_RPM),
                ElementsAre(0x5A, 0x00, 0x5A, 0x01));
    EXPECT_THAT(engineParameterCommand(EngineParameter::BATTERY_VOLTAGE),
                ElementsAre(0x5A, 0x0C));
    EXPECT_THROW({
        engineParameterCommand(static_cast<EngineParameter>(0xffu));
    }, std::invalid_argument);
}


TEST(ConsultEngineParametersTest, engineParameterDecode_one_byte_valid) {
    const std::vector<uint8_t> data {0x25, 0x99, 0x1c, 0x97, 0x99, 0x73, 0x75, 0x40};
    auto range = cmn::make_range(data);
    EXPECT_EQ(engineParameterDecode(EngineParameter::COOLANT_TEMPERATURE, range),
              -13.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_O2_SENSOR_VOLTAGE, range),
              1.53);
    EXPECT_EQ(engineParameterDecode(EngineParameter::VEHICLE_SPEED, range),
              56.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::BATTERY_VOLTAGE, range),
              12.08);
    EXPECT_EQ(engineParameterDecode(EngineParameter::THROTTLE_POSITION, range),
              3.06);
    EXPECT_EQ(engineParameterDecode(EngineParameter::IGNITION_TIMING, range),
              -5.0);
    EXPECT_EQ(engineParameterDecode(EngineParameter::AAC_VALVE, range),
              58.5);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_AIR_FUEL_ALPHA, range),
              64.0);
    EXPECT_TRUE(range.empty());
}

TEST(ConsultEngineParametersTest, engineParameterDecode_two_byte_valid) {
    const std::vector<uint8_t> data {0x01, 0x59, 0x02, 0x69, 0x11, 0xa2};
    auto range = cmn::make_range(data);
    EXPECT_EQ(engineParameterDecode(EngineParameter::ENGINE_RPM, range),
              4312.5);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_MAF_VOLTAGE, range),
              3.085);
    EXPECT_EQ(engineParameterDecode(EngineParameter::LH_INJECTION_TIMING, range),
              0.04514);
    EXPECT_TRUE(range.empty());
}

TEST(ConsultEngineParametersTest, engineParameterDecode_one_byte_invalid) {
    const std::vector<uint8_t> data{1, 2, 3, 4};
    auto empty_range = cmn::make_range(data.begin(), data.begin());
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::BATTERY_VOLTAGE, empty_range);
    }, std::invalid_argument);
    EXPECT_EQ(data.begin(), empty_range.begin()); // Range must not be modified
}

TEST(ConsultEngineParametersTest, engineParameterDecode_two_byte_invalid) {
    const std::vector<uint8_t> data {1, 2, 3, 4};
    auto empty_range = cmn::make_range(data.begin(), data.begin());
    auto one_byte_range = cmn::make_range(data.begin(), ++(data.begin()));
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::ENGINE_RPM, empty_range);
    }, std::invalid_argument);
    EXPECT_THROW({
        engineParameterDecode(EngineParameter::ENGINE_RPM, one_byte_range);
    }, std::invalid_argument);
    EXPECT_EQ(data.begin(), empty_range.begin()); // Range must not be modified
    EXPECT_EQ(data.begin(), one_byte_range.begin());
}

TEST(ConsultEngineParametersTest, engineParameterDecode_parameter_invalid) {
    const std::vector<uint8_t> data {1, 2, 3, 4};
    auto range = cmn::make_range(data);
    EXPECT_THROW({
        engineParameterDecode(static_cast<EngineParameter>(0xffu), range);
    }, std::invalid_argument);
    EXPECT_EQ(data.begin(), range.begin()); // Range must not be modified
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
