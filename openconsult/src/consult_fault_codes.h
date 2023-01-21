#ifndef OPENCONSULT_LIB_CONSULT_FAULT_CODES
#define OPENCONSULT_LIB_CONSULT_FAULT_CODES

#include <cstdint>
#include <string>

namespace openconsult {


/**
 * @brief A fault code used by the ECU to describe a fault.
 *
 * For engines with a two cylinder banks, the non-qualified fault codes refer to
 * the left-hand bank.
 */
enum class FaultCode : uint8_t {
    CRANKSHAFT_POSITION_SENSOR_CIRCUIT= 11,
    MASS_AIR_FLOW_SENSOR              = 12,
    ENGINE_COOLANT_SENSOR             = 13,
    VEHICLE_SPEED_SENSOR              = 14,
    MOTOR_THROTTLE_SWITCH             = 16,
    ABS_TCS_CONTROL_UNIT              = 17,
    IGNITION_SIGNAL                   = 21,
    FUEL_PUMP                         = 22,
    IDLE_SWITCH                       = 23,
    THROTTLE_VALVE_SWITCH             = 24,
    IDLE_SPEED_CONTROL                = 25,
    TURBO_PRESSURE_SENSOR             = 26,
    COOLANT_OVERHEAT                  = 28,
    ECM_FAULT                         = 31,
    EXHAUST_GAS_RECIRCULATION         = 32,
    HEATED_OXYGEN_SENSOR              = 33,
    KNOCK_SENSOR                      = 34,
    EXHAUST_GAS_TEMPERATURE_SENSOR    = 35,
    EGR_CONTROL_VALVE                 = 36,
    ENGINE_KNOCK                      = 37,
    CLOSED_LOOP_RH                    = 38,
    INTAKE_AIR_TEMPERATURE_SENSOR     = 41,
    FUEL_TEMPERATURE_SENSOR           = 42,
    THROTTLE_POSITION_SENSOR          = 43,
    ABS_TCS_CIRCUIT                   = 44,
    INJECTOR_LEAK                     = 45,
    THROTTLE_MOTOR_SENSOR             = 46,
    CRANKSHAFT_POSITION_SENSOR        = 47,
    FUEL_INJECTOR                     = 51,
    OXYGEN_SENSOR                     = 53,
    AUTOMATIC_TRANSMISSION            = 54,
    NO_MALFUNCTION                    = 55,
    CYLINDER_6_MISFIRE                = 63,
    CYLINDER_5_MISFIRE                = 64,
    CYLINDER_4_MISFIRE                = 65,
    CYLINDER_3_MISFIRE                = 66,
    CYLINDER_2_MISFIRE                = 67,
    CYLINDER_1_MISFIRE                = 68,
    RANDOM_MISFIRE                    = 71,
    THREE_WAY_CATALYST_RH             = 72,
    FUEL_INJECTOR_RH                  = 76,
    HEATED_OXYGEN_SENSOR_REAR         = 77,
    CRANKSHAFT_POSITION_SENSOR_RH     = 82,
    AUTOMATIC_TRANSMISSION_DIAGNOSTIC = 84,
    CANISTER_CONTROL_VALVE            = 87,
    HEATED_OXYGEN_SENSOR_RH           = 91,
    TORQUE_CONVERTER_VALVE            = 94,
    CRANK_POSITION_COG                = 95,
    ENGINE_COOLANT_SENSOR_RH          = 98,
    HEATED_OXYGEN_SENSOR_RH2          = 101,
    PARK_POSITION_SWITCH              = 103,
    EXHAUST_GAS_RECIRCULATION_VALVE   = 105,
    CANISTER_PURGE_CONTROL_VALVE      = 108,
    INHIBITOR_SWITCH                  = 111,
    AT_SPEED_SENSOR                   = 112,
    AT_1ST_SIGNAL                     = 113,
    AT_2ND_SIGNAL                     = 114,
    AT_3RD_SIGNAL                     = 115,
    AT_4TH_SIGNAL                     = 116,
    AT_SHIFT_VALVE_A                  = 118,
    AT_SHIFT_VALVE_B                  = 121,
    AT_OVERRUN_CLUTCH_VALVE           = 123,
    AT_TORQUE_CONVERTER_VALVE         = 124,
    AT_LINE_PRESSURE                  = 125,
    AT_THROTTLE_POSITION_SENSOR       = 126,
    AT_ENGINE_SPEED                   = 127,
    AT_TEMPERATURE_SENSOR             = 128,
};

/**
 * @brief Converts a raw ID, as supplied by an ECU, into a \c FaultCode .
 *
 * @param id The ID to convert.
 * @return The \c FaultCode represented by the ID.
 * @throws std::invalid_argument if \c id does not identify a known
 *      \c FaultCode .
 */
FaultCode faultCodeFromId(uint8_t id);

/**
 * @brief Converts a \c FaultCode into a raw ID, as supplied by an ECU.
 *
 * @param code The \c FaultCode to convert.
 * @return The ID used by the ECU to represent \c code .
 */
uint8_t faultCodeToId(FaultCode code);

/**
 * @brief Retrieves a short name for a \c FaultCode .
 *
 * This name is in English, may contain spaces but does not contain other
 * whitespace. Typically 60 characters or fewer.
 *
 * @param code The \c FaultCode to look-up.
 * @return The short name for \c code .
 * @throws std::invalid_argument if \c code is not valid.
 */
std::string faultCodeName(FaultCode code);

/**
 * @brief Retrieves a longer description for a \c FaultCode .
 *
 * This description is in English, may contain spaces but does not contain other
 * whitespace. May contain multiple sentences and always ends in a full-stop.
 * May be several hundred characters long. Some \c FaultCode s do not have an
 * available description, in which case the empty string is returned.
 *
 * @param code The \c FaultCode to look-up.
 * @return The description for \c code .
 * @throws std::invalid_argument if \c code is not valid.
 */
std::string faultCodeDescription(FaultCode code);


}

#endif