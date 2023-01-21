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
