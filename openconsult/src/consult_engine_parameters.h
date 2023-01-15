#ifndef OPENCONSULT_LIB_CONSULT_ENGINE_PARAMETERS
#define OPENCONSULT_LIB_CONSULT_ENGINE_PARAMETERS

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief An engine parameter that can be queried from the ECU.
 *
 * For engines with a single cylinder bank, use the LH_ parameter variants.
 */
enum class EngineParameter {
    ENGINE_RPM,
    LH_MAF_VOLTAGE,
    RH_MAF_VOLTAGE,
    COOLANT_TEMPERATURE,
    LH_O2_SENSOR_VOLTAGE,
    RH_O2_SENSOR_VOLTAGE,
    VEHICLE_SPEED,
    BATTERY_VOLTAGE,
    THROTTLE_POSITION,
    FUEL_TEMPERATURE,
    INTAKE_AIR_TEMPERATURE,
    EXHAUST_GAS_TEMPERATURE,
    LH_INJECTION_TIMING,
    RH_INJECTION_TIMING,
    IGNITION_TIMING,
    AAC_VALVE,
    LH_AIR_FUEL_ALPHA,
    RH_AIR_FUEL_ALPHA,
    LH_AIR_FUEL_ALPHA_SELF_LEARN,
    RH_AIR_FUEL_ALPHA_SELF_LEARN,
    MR_FC_MNT,
    WASTE_GATE_SOLENOID,
    TURBO_BOOST_SENSOR,
    ENGINE_MOUNT,
    POSITION_COUNTER,
    PURGE_CONTROL_VALVE,
    TANK_FUEL_TEMPERATURE,
    FPCM_DR_VOLTAGE,
    FUEL_GAUGE_VOLTAGE,
    DIGITAL_BIT_REGISTER1,
    DIGITAL_BIT_REGISTER2,
    DIGITAL_BIT_REGISTER3,
};

/**
 * @brief Retrieves the command byte sequence necessary to query a given
 *      \c EngineParameter from the ECU. Some parameters may have longer
 *      sequences than others.
 *
 * @param parameter The \c EngineParameter to look-up.
 * @return The byte sequence to query the parameter.
 * @throws std::invalid_argument if \c parameter is not valid.
 */
std::vector<uint8_t> engineParameterCommand(EngineParameter parameter);

/**
 * @brief Decodes a byte sequence, as returned when querying the ECU, into a
 *      real value for a particular \c EngineParameter . Some parameters may
 *      require more bytes to decode than others.
 *
 * @param parameter The \c EngineParameter to decode the byte sequence as.
 * @param data The byte sequence to decode.
 * @return Parameter value, in the unit described by the parameter.
 * @throws std::invalid_argument if \c parameter is not valid, or if the \c data
 *      vector is the wrong length to decode the requested parameter from.
 */
double engineParameterDecode(EngineParameter parameter, const std::vector<uint8_t>& data);

/**
 * @brief Retrieves a short name for an \c EngineParameter .
 *
 * This name is in English, may contain spaces but does not contain other
 * whitespace. Typically 60 characters or fewer.
 *
 * @param parameter The \c EngineParameter to look-up.
 * @return The short name for \c parameter .
 * @throws std::invalid_argument if \c parameter is not valid.
 */
std::string engineParameterName(EngineParameter parameter);

/**
 * @brief Retrieves a longer description for an \c EngineParameter .
 *
 * This description is in English, may contain spaces but does not contain other
 * whitespace. May contain multiple sentences and always ends in a full-stop.
 * May be several hundred characters long. Some \c EngineParameter s do not have
 * an available description, in which case the empty string is returned.
 *
 * @param parameter The \c EngineParameter to look-up.
 * @return The description for \c parameter .
 * @throws std::invalid_argument if \c parameter is not valid.
 */
std::string engineParameterDescription(EngineParameter parameter);

#endif