#include <initializer_list>
#include <stdexcept>
#include "common.h"
#include "consult_engine_parameters.h"

enum class RegisterIds : uint8_t {
    CRANKSHAFT_RPM_MSB              = 0x00,
    CRANKSHAFT_RPM_LSB              = 0x01,
    CAMSHAFT_RPM_MSB                = 0x02,
    CAMSHAFT_RPM_LSB                = 0x03,
    LH_MAF_VOLTAGE_MSB              = 0x04,
    LH_MAF_VOLTAGE_LSB              = 0x05,
    RH_MAF_VOLTAGE_MSB              = 0x06,
    RH_MAF_VOLTAGE_LSB              = 0x07,
    COOLANT_TEMP                    = 0x08,
    LH_O2_SENSOR_VOLTAGE            = 0x09,
    RH_O2_SENSOR_VOLTAGE            = 0x0A,
    VEHICLE_SPEED                   = 0x0B,
    BATTERY_VOLTAGE                 = 0x0C,
    THROTTLE_POSITION               = 0x0D,
    FUEL_TEMP                       = 0x0F,
    INTAKE_AIR_TEMP                 = 0x11,
    EXHAUST_GAS_TEMP                = 0x12,
    DIGITAL_BIT_REGISTER1           = 0x13,
    LH_INJECTION_TIMING_MSB         = 0x14,
    LH_INJECTION_TIMING_LSB         = 0x15,
    IGNITION_TIMING                 = 0x16,
    AAC_VALVE                       = 0x17,
    LH_AF_ALPHA                     = 0x1A,
    RH_AF_ALPHA                     = 0x1B,
    LH_AF_ALPHA_SELFLEARN           = 0x1C,
    RH_AF_ALPHA_SELFLEARN           = 0x1D,
    DIGITAL_BIT_REGISTER2           = 0x1E,
    DIGITAL_BIT_REGISTER3           = 0x1F,
    MR_FC_MNT                       = 0x21,
    RH_INJECTION_TIMING_MSB         = 0x22,
    RH_INJECTION_TIMING_LSB         = 0x23,
    WASTE_GATE_SOLENOID             = 0x28,
    TURBO_BOOST_SENSOR              = 0x29,
    ENGINE_MOUNT                    = 0x2A,
    POSITION_COUNTER                = 0x2E,
    PURGE_CONTROL_VALVE             = 0x25,
    TANK_FUEL_TEMP                  = 0x26,
    FPCM_DR_VOLTAGE                 = 0x27,
    FUEL_GAUGE_VOLTAGE              = 0x2F,
    FR_O2_HEATER_B1                 = 0x30,
    FR_O2_HEATER_B2                 = 0x31,
    IGNITION_SWITCH                 = 0x32,
    CAL_LD_VALUE                    = 0x33,
    BFUEL_SCHEDULE                  = 0x34,
    RR_O2_SENSOR_B1                 = 0x35,
    RR_O2_SENSOR_B2                 = 0x36,
    ABSOLUTE_THROTTLE_POSITION      = 0x37,
    MAF_UNKNOWN                     = 0x38,
    EVAP_SYSTEM_PRESSURE            = 0x39,
    ABSOLUTE_SYSTEM_PRESSURE_MSB    = 0x3A,
    ABSOLUTE_SYSTEM_PRESSURE_LSB    = 0x4A,
    FPCM_FP_VOLTAGE_MSB             = 0x52,
    FPCM_FP_VOLTAGE_LSB             = 0x53,
};

std::vector<uint8_t> registersToBytes(std::initializer_list<RegisterIds> params) {
    std::vector<uint8_t> bytes;
    for (auto param : params) {
        bytes.push_back(static_cast<uint8_t>(param));
    }
    return bytes;
}

std::vector<uint8_t> engineParameterCommand(EngineParameter parameter) {
    // Constructs the command byte sequence from an EngineParameter. Each
    // EngineParameter maps to one or more RegisterIds holding the data about
    // the parameter.
    // This approach also allows us to extend the mechanism to exposing memory-
    // mapped data in the future if we find a map that exposes data not already
    // covered by the registers.
    switch (parameter) {
        case EngineParameter::ENGINE_RPM:
            return registersToBytes({RegisterIds::CRANKSHAFT_RPM_MSB, RegisterIds::CRANKSHAFT_RPM_LSB});
        case EngineParameter::LH_MAF_VOLTAGE:
            return registersToBytes({RegisterIds::LH_MAF_VOLTAGE_MSB, RegisterIds::LH_MAF_VOLTAGE_LSB});
        case EngineParameter::RH_MAF_VOLTAGE:
            return registersToBytes({RegisterIds::RH_MAF_VOLTAGE_MSB, RegisterIds::RH_MAF_VOLTAGE_LSB});
        case EngineParameter::COOLANT_TEMPERATURE:
            return registersToBytes({RegisterIds::COOLANT_TEMP});
        case EngineParameter::LH_O2_SENSOR_VOLTAGE:
            return registersToBytes({RegisterIds::LH_O2_SENSOR_VOLTAGE});
        case EngineParameter::RH_O2_SENSOR_VOLTAGE:
            return registersToBytes({RegisterIds::RH_O2_SENSOR_VOLTAGE});
        case EngineParameter::VEHICLE_SPEED:
            return registersToBytes({RegisterIds::VEHICLE_SPEED});
        case EngineParameter::BATTERY_VOLTAGE:
            return registersToBytes({RegisterIds::BATTERY_VOLTAGE});
        case EngineParameter::THROTTLE_POSITION:
            return registersToBytes({RegisterIds::THROTTLE_POSITION});
        case EngineParameter::FUEL_TEMPERATURE:
            return registersToBytes({RegisterIds::FUEL_TEMP});
        case EngineParameter::INTAKE_AIR_TEMPERATURE:
            return registersToBytes({RegisterIds::INTAKE_AIR_TEMP});
        case EngineParameter::EXHAUST_GAS_TEMPERATURE:
            return registersToBytes({RegisterIds::EXHAUST_GAS_TEMP});
        case EngineParameter::LH_INJECTION_TIMING:
            return registersToBytes({RegisterIds::LH_INJECTION_TIMING_MSB, RegisterIds::LH_INJECTION_TIMING_LSB});
        case EngineParameter::RH_INJECTION_TIMING:
            return registersToBytes({RegisterIds::RH_INJECTION_TIMING_MSB, RegisterIds::RH_INJECTION_TIMING_LSB});
        case EngineParameter::IGNITION_TIMING:
            return registersToBytes({RegisterIds::IGNITION_TIMING});
        case EngineParameter::AAC_VALVE:
            return registersToBytes({RegisterIds::AAC_VALVE});
        case EngineParameter::LH_AIR_FUEL_ALPHA:
            return registersToBytes({RegisterIds::LH_AF_ALPHA});
        case EngineParameter::RH_AIR_FUEL_ALPHA:
            return registersToBytes({RegisterIds::RH_AF_ALPHA});
        case EngineParameter::LH_AIR_FUEL_ALPHA_SELF_LEARN:
            return registersToBytes({RegisterIds::LH_AF_ALPHA_SELFLEARN});
        case EngineParameter::RH_AIR_FUEL_ALPHA_SELF_LEARN:
            return registersToBytes({RegisterIds::RH_AF_ALPHA_SELFLEARN});
        case EngineParameter::MR_FC_MNT:
            return registersToBytes({RegisterIds::MR_FC_MNT});
        case EngineParameter::WASTE_GATE_SOLENOID:
            return registersToBytes({RegisterIds::WASTE_GATE_SOLENOID});
        case EngineParameter::TURBO_BOOST_SENSOR:
            return registersToBytes({RegisterIds::TURBO_BOOST_SENSOR});
        case EngineParameter::ENGINE_MOUNT:
            return registersToBytes({RegisterIds::ENGINE_MOUNT});
        case EngineParameter::POSITION_COUNTER:
            return registersToBytes({RegisterIds::POSITION_COUNTER});
        case EngineParameter::PURGE_CONTROL_VALVE:
            return registersToBytes({RegisterIds::PURGE_CONTROL_VALVE});
        case EngineParameter::TANK_FUEL_TEMPERATURE:
            return registersToBytes({RegisterIds::TANK_FUEL_TEMP});
        case EngineParameter::FPCM_DR_VOLTAGE:
            return registersToBytes({RegisterIds::FPCM_DR_VOLTAGE});
        case EngineParameter::FUEL_GAUGE_VOLTAGE:
            return registersToBytes({RegisterIds::FUEL_GAUGE_VOLTAGE});
        case EngineParameter::DIGITAL_BIT_REGISTER1:
            return registersToBytes({RegisterIds::DIGITAL_BIT_REGISTER1});
        case EngineParameter::DIGITAL_BIT_REGISTER2:
            return registersToBytes({RegisterIds::DIGITAL_BIT_REGISTER2});
        case EngineParameter::DIGITAL_BIT_REGISTER3:
            return registersToBytes({RegisterIds::DIGITAL_BIT_REGISTER3});
        default:
            std::string error = cmn::pformat("Unknown engine parameter: %02x", parameter);
            throw std::invalid_argument(error);
    }
}

void arg_assert(bool assertion) {
    if (!assertion) {
        throw std::invalid_argument("Invalid data length for engine parameter.");
    }
}

double engineParameterDecode(EngineParameter parameter, const std::vector<uint8_t>& data) {
    // For multi-byte responses, byte[0] is always the MSB, byte[1] is the LSB.
    switch (parameter) {
        case EngineParameter::ENGINE_RPM:               // RPM
            arg_assert(data.size() == 2);
            return ((data[0] << 8) | (data[1])) * 12.5;
        case EngineParameter::LH_MAF_VOLTAGE:           // V
        case EngineParameter::RH_MAF_VOLTAGE:
            arg_assert(data.size() == 2);
            return ((data[0] << 8) | (data[1])) * 5 * 0.001;
        case EngineParameter::COOLANT_TEMPERATURE:      // deg C
        case EngineParameter::FUEL_TEMPERATURE:
        case EngineParameter::INTAKE_AIR_TEMPERATURE:
        case EngineParameter::TANK_FUEL_TEMPERATURE:
            arg_assert(data.size() == 1);
            return data[0] - 50;
        case EngineParameter::LH_O2_SENSOR_VOLTAGE:     // V
        case EngineParameter::RH_O2_SENSOR_VOLTAGE:
            arg_assert(data.size() == 1);
            return data[0] * 10 * 0.001;
        case EngineParameter::VEHICLE_SPEED:            // KM/H
            arg_assert(data.size() == 1);
            return data[0] * 2;
        case EngineParameter::BATTERY_VOLTAGE:          // V
            arg_assert(data.size() == 1);
            return data[0] * 80 * 0.001;
        case EngineParameter::THROTTLE_POSITION:        // V
        case EngineParameter::EXHAUST_GAS_TEMPERATURE:
            arg_assert(data.size() == 1);
            return data[0] * 20 * 0.001;
        case EngineParameter::LH_INJECTION_TIMING:      // S
        case EngineParameter::RH_INJECTION_TIMING:
            arg_assert(data.size() == 2);
            return ((data[0] << 8) | (data[1])) * 0.01 * 0.001;
        case EngineParameter::IGNITION_TIMING:          // deg BTDC
            arg_assert(data.size() == 1);
            return 110.0 - data[0];
        case EngineParameter::AAC_VALVE:                // %
            arg_assert(data.size() == 1);
            return data[0] / 2.0;
        case EngineParameter::LH_AIR_FUEL_ALPHA:        // %
        case EngineParameter::RH_AIR_FUEL_ALPHA:
        case EngineParameter::LH_AIR_FUEL_ALPHA_SELF_LEARN:
        case EngineParameter::RH_AIR_FUEL_ALPHA_SELF_LEARN:
        case EngineParameter::WASTE_GATE_SOLENOID:
            arg_assert(data.size() == 1);
            return data[0];
        case EngineParameter::MR_FC_MNT:                // RICH/LEAN
            arg_assert(data.size() == 1);
            return data[0];
        case EngineParameter::TURBO_BOOST_SENSOR:       // V
        case EngineParameter::FPCM_DR_VOLTAGE:
        case EngineParameter::FUEL_GAUGE_VOLTAGE:
            // TODO: All these voltages have unknown scaling. It's likely x20
            // based on the other single-byte mV register scalings, but this is
            // a guess.
            arg_assert(data.size() == 1);
            return data[0] * 20 * 0.001;
        case EngineParameter::ENGINE_MOUNT:             // ??
        case EngineParameter::POSITION_COUNTER:
        case EngineParameter::PURGE_CONTROL_VALVE:
            // TODO: All these parameters track an unknown quantity.
            arg_assert(data.size() == 1);
            return data[0];
        case EngineParameter::DIGITAL_BIT_REGISTER1:    // bit regs
        case EngineParameter::DIGITAL_BIT_REGISTER2:
        case EngineParameter::DIGITAL_BIT_REGISTER3:
            // TODO: The bit registers really need breaking out separately.
            arg_assert(data.size() == 1);
            return data[0];
        default:
            std::string error = cmn::pformat("Unknown engine parameter: %02x", parameter);
            throw std::invalid_argument(error);
    };
}

std::string engineParameterName(EngineParameter parameter) {
    switch (parameter) {
        case EngineParameter::ENGINE_RPM:
            return "Engine speed (RPM)";
        case EngineParameter::LH_MAF_VOLTAGE:
            return "Mass Air Flow meter voltage (V)";
        case EngineParameter::RH_MAF_VOLTAGE:
            return "Mass Air Flow meter voltage (right-hand bank) (V)";
        case EngineParameter::COOLANT_TEMPERATURE:
            return "Engine coolant temperature (deg C)";
        case EngineParameter::LH_O2_SENSOR_VOLTAGE:
            return "O2 Sensor voltage (V)";
        case EngineParameter::RH_O2_SENSOR_VOLTAGE:
            return "O2 Sensor voltage (right-hand bank) (V)";
        case EngineParameter::VEHICLE_SPEED:
            return "Vehicle speed (km/h)";
        case EngineParameter::BATTERY_VOLTAGE:
            return "Battery voltage (V)";
        case EngineParameter::THROTTLE_POSITION:
            return "Throttle Position Sensor (V)";
        case EngineParameter::FUEL_TEMPERATURE:
            return "Fuel temperature (deg C)";
        case EngineParameter::INTAKE_AIR_TEMPERATURE:
            return "Intake air temperature (deg C)";
        case EngineParameter::EXHAUST_GAS_TEMPERATURE:
            return "Exhaust gas temperature voltage (V)";
        case EngineParameter::LH_INJECTION_TIMING:
            return "Injection timing (S)";
        case EngineParameter::RH_INJECTION_TIMING:
            return "Injection timing (right-hand bank) (S)";
        case EngineParameter::IGNITION_TIMING:
            return "Ignition timing (deg BTDC)";
        case EngineParameter::AAC_VALVE:
            return "Idle air control valve (%)";
        case EngineParameter::LH_AIR_FUEL_ALPHA:
            return "Air/Fuel alpha (%)";
        case EngineParameter::RH_AIR_FUEL_ALPHA:
            return "Air/Fuel alpha (right-hand bank) (%)";
        case EngineParameter::LH_AIR_FUEL_ALPHA_SELF_LEARN:
            return "Air/Fuel alpha (self learn) (%)";
        case EngineParameter::RH_AIR_FUEL_ALPHA_SELF_LEARN:
            return "Air/Fuel alpha (right-hand bank) (self learn) (%)";
        case EngineParameter::MR_FC_MNT:
            return "Air/Fuel mixture ratio feedback control (rich/lean)";
        case EngineParameter::WASTE_GATE_SOLENOID:
            return "Waste gate solenoid (%)";
        case EngineParameter::TURBO_BOOST_SENSOR:
            return "Turbo boost sensor (V)";
        case EngineParameter::ENGINE_MOUNT:
            return "Engine mount (on/off)";
        case EngineParameter::POSITION_COUNTER:
            return "Position Counter (count)";
        case EngineParameter::PURGE_CONTROL_VALVE:
            return "EVAP Purge Volume Control Valve (step)";
        case EngineParameter::TANK_FUEL_TEMPERATURE:
            return "Tank fuel temperature (deg C)";
        case EngineParameter::FPCM_DR_VOLTAGE:
            return "Fuel Pump Control Module";
        case EngineParameter::FUEL_GAUGE_VOLTAGE:
            return "Fuel gauge voltage (V)";
        case EngineParameter::DIGITAL_BIT_REGISTER1:
            return "Digital Bit Register 1";
        case EngineParameter::DIGITAL_BIT_REGISTER2:
            return "Digital Bit Register 2";
        case EngineParameter::DIGITAL_BIT_REGISTER3:
            return "Digital Bit Register 3";
        default:
            std::string error = cmn::pformat("Unknown engine parameter: %02x", parameter);
            throw std::invalid_argument(error);
    };
}

std::string engineParameterDescription(EngineParameter parameter) {
    switch (parameter) {
        case EngineParameter::ENGINE_RPM:
            return "The engine speed computed from the REF signal (180 degree signal) of the camshaft position sensor.";
        case EngineParameter::LH_MAF_VOLTAGE:
            return "The signal voltage of the mass air flow sensor.";
        case EngineParameter::RH_MAF_VOLTAGE:
            return "The signal voltage of the right-hand mass air flow sensor.";
        case EngineParameter::COOLANT_TEMPERATURE:
            return "The temperature of the engine coolant.";
        case EngineParameter::LH_O2_SENSOR_VOLTAGE:
            return "The signal voltage of the heated oxygen sensor 1 (front).";
        case EngineParameter::RH_O2_SENSOR_VOLTAGE:
            return "The signal voltage of the right-hand heated oxygen sensor 1 (front).";
        case EngineParameter::VEHICLE_SPEED:
            return "The vehicle speed";
        case EngineParameter::BATTERY_VOLTAGE:
            return "The power supply voltage of the ECM.";
        case EngineParameter::THROTTLE_POSITION:
            return "The signal voltage of the throttle position sensor.";
        case EngineParameter::FUEL_TEMPERATURE:
            return "The temperature of the fuel in the fuel rail.";
        case EngineParameter::INTAKE_AIR_TEMPERATURE:
            return "The temperature of the intake air.";
        case EngineParameter::EXHAUST_GAS_TEMPERATURE:
            return "The signal voltage of the exhaust gas temperature sensor.";
        case EngineParameter::LH_INJECTION_TIMING:
            return "The actual fuel injection pulse width, compensated by the ECM."; // sp. probably means 'computed by ECM'
        case EngineParameter::RH_INJECTION_TIMING:
            return "The actual fuel injection pulse width of the right-hand injectors, compensated by the ECM.";
        case EngineParameter::IGNITION_TIMING:
            return "The ignition timing.";
        case EngineParameter::AAC_VALVE:
            return "The IACV-AAC valve control value signal.";
        case EngineParameter::LH_AIR_FUEL_ALPHA:
            return "The mean value of the air-fuel ratio feedback correction factor per cycle.";
        case EngineParameter::RH_AIR_FUEL_ALPHA:
            return "The mean value of the air-fuel ratio feedback correction factor per cycle for the right-hand cylinder bank.";
        case EngineParameter::LH_AIR_FUEL_ALPHA_SELF_LEARN:
            return "The mean value of the air-fuel ratio feedback correction factor per cycle, as used for the air-fuel ratio learning control.";
        case EngineParameter::RH_AIR_FUEL_ALPHA_SELF_LEARN:
            return "The mean value of the air-fuel ratio feedback correction factor per cycle, as used for the air-fuel ratio learning control for the right-hand cylinder bank.";
        case EngineParameter::MR_FC_MNT:
            return "The front heated oxygen sensor signal during air-fuel ratio feedback control. May be 'RICH' or 'LEAN'. 'RICH' means the mixture became rich, and control is being affected towards a leaner mixture. 'LEAN' means the mixture became lean, and control is being affected towards a rich mixture.";
        case EngineParameter::WASTE_GATE_SOLENOID:
            return "The wastegate valve control solenoid signal.";
        case EngineParameter::TURBO_BOOST_SENSOR:
            return "";
        case EngineParameter::ENGINE_MOUNT:
            return "";
        case EngineParameter::POSITION_COUNTER:
            return "";
        case EngineParameter::PURGE_CONTROL_VALVE:
            return "";
        case EngineParameter::TANK_FUEL_TEMPERATURE:
            return "The temperature of the fuel in the fuel tank.";
        case EngineParameter::FPCM_DR_VOLTAGE:
            return "";
        case EngineParameter::FUEL_GAUGE_VOLTAGE:
            return "The signal voltage of the fuel gauge.";
        case EngineParameter::DIGITAL_BIT_REGISTER1:
            return "";
        case EngineParameter::DIGITAL_BIT_REGISTER2:
            return "";
        case EngineParameter::DIGITAL_BIT_REGISTER3:
            return "";
        default:
            std::string error = cmn::pformat("Unknown engine parameter: %02x", parameter);
            throw std::invalid_argument(error);
    };
}
