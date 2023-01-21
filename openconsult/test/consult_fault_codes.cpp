#include "openconsult/src/consult_fault_codes.h"
#include "openconsult/src/consult_fault_codes.internal.h"

#include <gtest/gtest.h>

using namespace openconsult;


TEST(ConsultFaultCodesTest, faultCodeFromId) {
    EXPECT_EQ(faultCodeFromId(22u), FaultCode::FUEL_PUMP);
    EXPECT_EQ(faultCodeFromId(55u), FaultCode::NO_MALFUNCTION);
    EXPECT_THROW({
        faultCodeFromId(0xffu);
    }, std::invalid_argument);
}


TEST(ConsultFaultCodesTest, faultCodeToId) {
    EXPECT_EQ(faultCodeToId(FaultCode::FUEL_PUMP), 22u);
    EXPECT_EQ(faultCodeToId(FaultCode::NO_MALFUNCTION), 55u);
}


TEST(ConsultFaultCodesTest, faultCodeName) {
    EXPECT_EQ(faultCodeName(FaultCode::FUEL_PUMP),      "Fuel pump");
    EXPECT_EQ(faultCodeName(FaultCode::NO_MALFUNCTION), "No malfunction");
    EXPECT_THROW({
        faultCodeName(static_cast<FaultCode>(0xffu));
    }, std::invalid_argument);
}


TEST(ConsultFaultCodesTest, faultCodeDescription) {
    EXPECT_EQ(faultCodeDescription(FaultCode::FUEL_PUMP),
              "");
    EXPECT_EQ(faultCodeDescription(FaultCode::NO_MALFUNCTION),
              "No malfunction is detected in any monitored circuit.");
    EXPECT_THROW({
        faultCodeDescription(static_cast<FaultCode>(0xffu));
    }, std::invalid_argument);
}
