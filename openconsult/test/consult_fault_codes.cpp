#include <gtest/gtest.h>
#include "openconsult/src/consult_fault_codes.h"

TEST(ConsultFaultCodesTest, faultCodeFromId) {
    EXPECT_EQ(FaultCode::FUEL_PUMP,      faultCodeFromId(22u));
    EXPECT_EQ(FaultCode::NO_MALFUNCTION, faultCodeFromId(55u));
    EXPECT_THROW({
        faultCodeFromId(255u);
    }, std::invalid_argument);
}

TEST(ConsultFaultCodesTest, faultCodeToId) {
    EXPECT_EQ(22u, faultCodeToId(FaultCode::FUEL_PUMP));
    EXPECT_EQ(55u, faultCodeToId(FaultCode::NO_MALFUNCTION));
}

TEST(ConsultFaultCodesTest, faultCodeName) {
    EXPECT_EQ("Fuel Pump",      faultCodeName(FaultCode::FUEL_PUMP));
    EXPECT_EQ("No malfunction", faultCodeName(FaultCode::NO_MALFUNCTION));
    EXPECT_THROW({
        faultCodeName(static_cast<FaultCode>(255u));
    }, std::invalid_argument);
}

TEST(ConsultFaultCodesTest, faultCodeDescription) {
    EXPECT_EQ("",
              faultCodeDescription(FaultCode::FUEL_PUMP));
    EXPECT_EQ("No malfunction is detected in any monitored circuit.",
              faultCodeDescription(FaultCode::NO_MALFUNCTION));
    EXPECT_THROW({
        faultCodeDescription(static_cast<FaultCode>(255u));
    }, std::invalid_argument);
}
