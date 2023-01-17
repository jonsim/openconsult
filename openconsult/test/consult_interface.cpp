#include <gtest/gtest.h>
#include "openconsult/src/consult_interface.h"

TEST(ECUPartNumberTest, toJSON) {
    ECUPartNumber part_number("1480 23710-353032");
    EXPECT_EQ("{\n"
              "  \"part_number\": \"1480 23710-353032\"\n"
              "}", part_number.toJSON());
}


TEST(FaultCodeDataTest, toJSON) {
    FaultCodeData code(FaultCode::FUEL_INJECTOR, 42);
    EXPECT_EQ("{\n"
              "  \"code\": 51,\n"
              "  \"name\": \"Injector Circuit\",\n"
              "  \"description\": null,\n"
              "  \"starts_since_observed\": 42\n"
              "}", code.toJSON());
}


TEST(FaultCodesTest, toJSON) {
    FaultCodes codes({{FaultCode::KNOCK_SENSOR, 13},
                      {FaultCode::INJECTOR_LEAK, 17}});
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
    EngineParameters parameters({std::make_pair(EngineParameter::ENGINE_RPM, 1234.0),
                                 std::make_pair(EngineParameter::BATTERY_VOLTAGE, 14.86)});
    EXPECT_EQ("{\n"
              "  \"engine_speed_rpm\": 1234.00,\n"
              "  \"battery_v\": 14.86\n"
              "}", parameters.toJSON());
}
