#include "consult_fault_codes.h"
#include "common.h"

#include <stdexcept>

namespace openconsult {


FaultCode faultCodeFromId(uint8_t id) {
    FaultCode code = static_cast<FaultCode>(id);
    switch (code) {
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_CIRCUIT:
        case FaultCode::MASS_AIR_FLOW_SENSOR:
        case FaultCode::ENGINE_COOLANT_SENSOR:
        case FaultCode::VEHICLE_SPEED_SENSOR:
        case FaultCode::MOTOR_THROTTLE_SWITCH:
        case FaultCode::ABS_TCS_CONTROL_UNIT:
        case FaultCode::IGNITION_SIGNAL:
        case FaultCode::FUEL_PUMP:
        case FaultCode::IDLE_SWITCH:
        case FaultCode::THROTTLE_VALVE_SWITCH:
        case FaultCode::IDLE_SPEED_CONTROL:
        case FaultCode::TURBO_PRESSURE_SENSOR:
        case FaultCode::COOLANT_OVERHEAT:
        case FaultCode::ECM_FAULT:
        case FaultCode::EXHAUST_GAS_RECIRCULATION:
        case FaultCode::HEATED_OXYGEN_SENSOR:
        case FaultCode::KNOCK_SENSOR:
        case FaultCode::EXHAUST_GAS_TEMPERATURE_SENSOR:
        case FaultCode::EGR_CONTROL_VALVE:
        case FaultCode::ENGINE_KNOCK:
        case FaultCode::CLOSED_LOOP_RH:
        case FaultCode::INTAKE_AIR_TEMPERATURE_SENSOR:
        case FaultCode::FUEL_TEMPERATURE_SENSOR:
        case FaultCode::THROTTLE_POSITION_SENSOR:
        case FaultCode::ABS_TCS_CIRCUIT:
        case FaultCode::INJECTOR_LEAK:
        case FaultCode::THROTTLE_MOTOR_SENSOR:
        case FaultCode::CRANKSHAFT_POSITION_SENSOR:
        case FaultCode::FUEL_INJECTOR:
        case FaultCode::OXYGEN_SENSOR:
        case FaultCode::AUTOMATIC_TRANSMISSION:
        case FaultCode::NO_MALFUNCTION:
        case FaultCode::CYLINDER_6_MISFIRE:
        case FaultCode::CYLINDER_5_MISFIRE:
        case FaultCode::CYLINDER_4_MISFIRE:
        case FaultCode::CYLINDER_3_MISFIRE:
        case FaultCode::CYLINDER_2_MISFIRE:
        case FaultCode::CYLINDER_1_MISFIRE:
        case FaultCode::RANDOM_MISFIRE:
        case FaultCode::THREE_WAY_CATALYST_RH:
        case FaultCode::FUEL_INJECTOR_RH:
        case FaultCode::HEATED_OXYGEN_SENSOR_REAR:
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_RH:
        case FaultCode::AUTOMATIC_TRANSMISSION_DIAGNOSTIC:
        case FaultCode::CANISTER_CONTROL_VALVE:
        case FaultCode::HEATED_OXYGEN_SENSOR_RH:
        case FaultCode::TORQUE_CONVERTER_VALVE:
        case FaultCode::CRANK_POSITION_COG:
        case FaultCode::ENGINE_COOLANT_SENSOR_RH:
        case FaultCode::HEATED_OXYGEN_SENSOR_RH2:
        case FaultCode::PARK_POSITION_SWITCH:
        case FaultCode::EXHAUST_GAS_RECIRCULATION_VALVE:
        case FaultCode::CANISTER_PURGE_CONTROL_VALVE:
        case FaultCode::INHIBITOR_SWITCH:
        case FaultCode::AT_SPEED_SENSOR:
        case FaultCode::AT_1ST_SIGNAL:
        case FaultCode::AT_2ND_SIGNAL:
        case FaultCode::AT_3RD_SIGNAL:
        case FaultCode::AT_4TH_SIGNAL:
        case FaultCode::AT_SHIFT_VALVE_A:
        case FaultCode::AT_SHIFT_VALVE_B:
        case FaultCode::AT_OVERRUN_CLUTCH_VALVE:
        case FaultCode::AT_TORQUE_CONVERTER_VALVE:
        case FaultCode::AT_LINE_PRESSURE:
        case FaultCode::AT_THROTTLE_POSITION_SENSOR:
        case FaultCode::AT_ENGINE_SPEED:
        case FaultCode::AT_TEMPERATURE_SENSOR:
            return code;
        default:
            std::string error = cmn::pformat("Unknown fault code: %02x", code);
            throw std::invalid_argument(error);
    }
}

uint8_t faultCodeToId(FaultCode code) {
    return static_cast<uint8_t>(code);
}

std::string faultCodeName(FaultCode code) {
    switch (code) {
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_CIRCUIT:
            return "Crankshaft position sensor signal circuit";
        case FaultCode::MASS_AIR_FLOW_SENSOR:
            return "Mass air flow sensor signal circuit";
        case FaultCode::ENGINE_COOLANT_SENSOR:
            return "Engine coolant temperature sensor signal circuit";
        case FaultCode::VEHICLE_SPEED_SENSOR:
            return "Vehicle speed sensor signal circuit";
        case FaultCode::MOTOR_THROTTLE_SWITCH:
            return "Motor throttle switch signal circuit [RB25DET]";
        case FaultCode::ABS_TCS_CONTROL_UNIT:
            return "ABS-TCS control unit circuit [RB25DET]";
        case FaultCode::IGNITION_SIGNAL:
            return "Ignition signal circuit";
        case FaultCode::FUEL_PUMP:
            return "Fuel pump";
        case FaultCode::IDLE_SWITCH:
            return "Idle switch";
        case FaultCode::THROTTLE_VALVE_SWITCH:
            return "Throttle Valve Switch";
        case FaultCode::IDLE_SPEED_CONTROL:
            return "Idle Speed Control Valve";
        case FaultCode::TURBO_PRESSURE_SENSOR:
            return "Turbo pressure sensor signal circuit [RB25DET]";
        case FaultCode::COOLANT_OVERHEAT:
            return "Overheat";
        case FaultCode::ECM_FAULT:
            return "ECM";
        case FaultCode::EXHAUST_GAS_RECIRCULATION:
            return "EGR Function";
        case FaultCode::HEATED_OXYGEN_SENSOR:
            return "Heated oxygen sensor signal circuit";
        case FaultCode::KNOCK_SENSOR:
            return "Knock sensor signal circuit";
        case FaultCode::EXHAUST_GAS_TEMPERATURE_SENSOR:
            return "Exhaust Gas Temperature Sensor";
        case FaultCode::EGR_CONTROL_VALVE:
            return "EGR Control-Back Pressure Transducer";
        case FaultCode::ENGINE_KNOCK:
            return "Knock Sensor";
        case FaultCode::CLOSED_LOOP_RH:
            return "Right hand bank Closed Loop (B2)";
        case FaultCode::INTAKE_AIR_TEMPERATURE_SENSOR:
            return "Intake Air Temperature Sensor";
        case FaultCode::FUEL_TEMPERATURE_SENSOR:
            return "Fuel Temperature Sensor";
        case FaultCode::THROTTLE_POSITION_SENSOR:
            return "Throttle position sensor signal circuit";
        case FaultCode::ABS_TCS_CIRCUIT:
            return "ABS-TCS communication circuit [RB25DET]";
        case FaultCode::INJECTOR_LEAK:
            return "Injector Leak";
        case FaultCode::THROTTLE_MOTOR_SENSOR:
            return "Throttle motor sensor signal circuit [RB25DET]";
        case FaultCode::CRANKSHAFT_POSITION_SENSOR:
            return "Crankshaft Position Sensor";
        case FaultCode::FUEL_INJECTOR:
            return "Injector Circuit";
        case FaultCode::OXYGEN_SENSOR:
            return "Oxygen Sensor";
        case FaultCode::AUTOMATIC_TRANSMISSION:
            return "A/T communication circuit";
        case FaultCode::NO_MALFUNCTION:
            return "No malfunction";
        case FaultCode::CYLINDER_6_MISFIRE:
            return "No. 6 Cylinder Misfire";
        case FaultCode::CYLINDER_5_MISFIRE:
            return "No. 5 Cylinder Misfire";
        case FaultCode::CYLINDER_4_MISFIRE:
            return "No. 4 Cylinder Misfire";
        case FaultCode::CYLINDER_3_MISFIRE:
            return "No. 3 Cylinder Misfire";
        case FaultCode::CYLINDER_2_MISFIRE:
            return "No. 2 Cylinder Misfire";
        case FaultCode::CYLINDER_1_MISFIRE:
            return "No. 1 Cylinder Misfire";
        case FaultCode::RANDOM_MISFIRE:
            return "Random Misfire";
        case FaultCode::THREE_WAY_CATALYST_RH:
            return "TWC Function right hand bank";
        case FaultCode::FUEL_INJECTOR_RH:
            return "Fuel Injection System Function right hand bank";
        case FaultCode::HEATED_OXYGEN_SENSOR_REAR:
            return "Rear Heated Oxygen Sensor Circuit";
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_RH:
            return "Crankshaft Position Sensor";
        case FaultCode::AUTOMATIC_TRANSMISSION_DIAGNOSTIC:
            return "Automatic Transmission Diagnostic Communication Line";
        case FaultCode::CANISTER_CONTROL_VALVE:
            return "Canister Control Solenoid Valve Circuit";
        case FaultCode::HEATED_OXYGEN_SENSOR_RH:
            return "Front Heated Oxygen Sensor Heater Circuit right hand bank";
        case FaultCode::TORQUE_CONVERTER_VALVE:
            return "TCC Solenoid Valve";
        case FaultCode::CRANK_POSITION_COG:
            return "Crank Position Cog";
        case FaultCode::ENGINE_COOLANT_SENSOR_RH:
            return "Engine Coolant Temperature Sensor";
        case FaultCode::HEATED_OXYGEN_SENSOR_RH2:
            return "Front Heated Oxygen Sensor Heater Circuit right hand bank";
        case FaultCode::PARK_POSITION_SWITCH:
            return "Park/Neutral Position Switch Circuit";
        case FaultCode::EXHAUST_GAS_RECIRCULATION_VALVE:
            return "EGR and EGR Canister Control Solenoid Valve Circuit";
        case FaultCode::CANISTER_PURGE_CONTROL_VALVE:
            return "Canister Purge Control Valve Circuit";
        case FaultCode::INHIBITOR_SWITCH:
            return "Inhibitor Switch";
        case FaultCode::AT_SPEED_SENSOR:
            return "Automatic Transmission Speed Sensor";
        case FaultCode::AT_1ST_SIGNAL:
            return "Automatic Transmission 1st Signal";
        case FaultCode::AT_2ND_SIGNAL:
            return "Automatic Transmission 2nd Signal";
        case FaultCode::AT_3RD_SIGNAL:
            return "Automatic Transmission 3rd Signal";
        case FaultCode::AT_4TH_SIGNAL:
            return "Automatic Transmission 4th Signal";
        case FaultCode::AT_SHIFT_VALVE_A:
            return "Automatic Transmission Shift Solenoid Valve A";
        case FaultCode::AT_SHIFT_VALVE_B:
            return "Automatic Transmission Shift Solenoid Valve B";
        case FaultCode::AT_OVERRUN_CLUTCH_VALVE:
            return "Automatic Transmission Overrun Clutch Solenoid Valve";
        case FaultCode::AT_TORQUE_CONVERTER_VALVE:
            return "Automatic Transmission TOR Conv Solenoid Valve";
        case FaultCode::AT_LINE_PRESSURE:
            return "Automatic Transmission Line Pressure Solenoid";
        case FaultCode::AT_THROTTLE_POSITION_SENSOR:
            return "Automatic Transmission Throttle Position Sensor";
        case FaultCode::AT_ENGINE_SPEED:
            return "Automatic Transmission Engine Speed Signal";
        case FaultCode::AT_TEMPERATURE_SENSOR:
            return "Automatic Transmission Fluid Temperature Sensor";
        default:
            std::string error = cmn::pformat("Unknown fault code: %02x", code);
            throw std::invalid_argument(error);
    }
}

std::string faultCodeDescription(FaultCode code) {
    switch (code) {
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_CIRCUIT:
            return "1-degree (POS) signal or 120-degree (REF) signal is not input for predetermined time while the engine is running. Abnormal correlation is detected between 1-degree (POS) signal and 120-degree (REF) sianal.";
        case FaultCode::MASS_AIR_FLOW_SENSOR:
            return "Mass air flow sensor output voltage is 4.9V or greater for predetermined time when ignition switch is turned from OFF to ON, or after the engine is stalled. Mass air flow sensor output voltage is less than 0.3V for predetermined time while the engine is running.";
        case FaultCode::ENGINE_COOLANT_SENSOR:
            return "Engine coolant temperature sensor output voltage is approx. 4.8V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time.";
        case FaultCode::VEHICLE_SPEED_SENSOR:
            return "No vehicle speed signal is input for predetermined time while the vehicle is being driven after warm up.";
        case FaultCode::MOTOR_THROTTLE_SWITCH:
            return "Abnormal correlation is detected between input voltages from the throttle motor sensor and from the motor throttle switch for predetermined time.";
        case FaultCode::ABS_TCS_CONTROL_UNIT:
            return "Throttle control unit detects malfunction in the system. (Open throttle sensor harness, etc.). TCS/ABS control unit detects malfunction.";
        case FaultCode::IGNITION_SIGNAL:
            return "No consecutive ignition signal while the engine is running.";
        case FaultCode::FUEL_PUMP:
            return "";
        case FaultCode::IDLE_SWITCH:
            return "";
        case FaultCode::THROTTLE_VALVE_SWITCH:
            return "";
        case FaultCode::IDLE_SPEED_CONTROL:
            return "";
        case FaultCode::TURBO_PRESSURE_SENSOR:
            return "Turbo pressure sensor output voltage is approx. 4.8V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time.";
        case FaultCode::COOLANT_OVERHEAT:
            return "Engine coolant temperature sensor output voltage is approx. 0.35V or less (sensor normal) for predetermined time.";
        case FaultCode::ECM_FAULT:
            return "";
        case FaultCode::EXHAUST_GAS_RECIRCULATION:
            return "";
        case FaultCode::HEATED_OXYGEN_SENSOR:
            return "Heated oxygen sensor output voltage is approx. 0.2V or greater and less than approx. 0.4V for predetermined time while the vehicle is being driven after warm up. Heated oxygen sensor output voltage is approx. 2V or greater for predetermined time.";
        case FaultCode::KNOCK_SENSOR:
            return "At least one knock sensor indicates the output voltage of approx. 4V or greater (open circuit) or less than approx. 1V (short circuit).";
        case FaultCode::EXHAUST_GAS_TEMPERATURE_SENSOR:
            return "";
        case FaultCode::EGR_CONTROL_VALVE:
            return "";
        case FaultCode::ENGINE_KNOCK:
            return "";
        case FaultCode::CLOSED_LOOP_RH:
            return "";
        case FaultCode::INTAKE_AIR_TEMPERATURE_SENSOR:
            return "";
        case FaultCode::FUEL_TEMPERATURE_SENSOR:
            return "";
        case FaultCode::THROTTLE_POSITION_SENSOR:
            return "Throttle position sensor output voltage is approx. 4.7V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time while park/neutral position switch is OFF and vehicle speed is 4 km/h or higher.";
        case FaultCode::ABS_TCS_CIRCUIT:
            return "Malfunction (open/short circuit, etc.) is detected in multiplex communication line between engine and TCS/ABS.";
        case FaultCode::INJECTOR_LEAK:
            return "";
        case FaultCode::THROTTLE_MOTOR_SENSOR:
            return "Throttle motor sensor input voltage is approx. 4.8V or greater (open circuit) or less than 0.3V (short circuit) for predetermined time.";
        case FaultCode::CRANKSHAFT_POSITION_SENSOR:
            return "";
        case FaultCode::FUEL_INJECTOR:
            return "";
        case FaultCode::OXYGEN_SENSOR:
            return "";
        case FaultCode::AUTOMATIC_TRANSMISSION:
            return "Malfunction is detected in PA communication circuit in ECM(-TCM). [RB25DE (L/B), RB25DET]. Malfunction (open circuit, short circuit, etc.) is detected in multiplex communication line between ECM and TCM. [RB25DE].";
        case FaultCode::NO_MALFUNCTION:
            return "No malfunction is detected in any monitored circuit.";
        case FaultCode::CYLINDER_6_MISFIRE:
            return "";
        case FaultCode::CYLINDER_5_MISFIRE:
            return "";
        case FaultCode::CYLINDER_4_MISFIRE:
            return "";
        case FaultCode::CYLINDER_3_MISFIRE:
            return "";
        case FaultCode::CYLINDER_2_MISFIRE:
            return "";
        case FaultCode::CYLINDER_1_MISFIRE:
            return "";
        case FaultCode::RANDOM_MISFIRE:
            return "";
        case FaultCode::THREE_WAY_CATALYST_RH:
            return "";
        case FaultCode::FUEL_INJECTOR_RH:
            return "";
        case FaultCode::HEATED_OXYGEN_SENSOR_REAR:
            return "";
        case FaultCode::CRANKSHAFT_POSITION_SENSOR_RH:
            return "";
        case FaultCode::AUTOMATIC_TRANSMISSION_DIAGNOSTIC:
            return "";
        case FaultCode::CANISTER_CONTROL_VALVE:
            return "";
        case FaultCode::HEATED_OXYGEN_SENSOR_RH:
            return "";
        case FaultCode::TORQUE_CONVERTER_VALVE:
            return "";
        case FaultCode::CRANK_POSITION_COG:
            return "";
        case FaultCode::ENGINE_COOLANT_SENSOR_RH:
            return "";
        case FaultCode::HEATED_OXYGEN_SENSOR_RH2:
            return "";
        case FaultCode::PARK_POSITION_SWITCH:
            return "";
        case FaultCode::EXHAUST_GAS_RECIRCULATION_VALVE:
            return "";
        case FaultCode::CANISTER_PURGE_CONTROL_VALVE:
            return "";
        case FaultCode::INHIBITOR_SWITCH:
            return "";
        case FaultCode::AT_SPEED_SENSOR:
            return "";
        case FaultCode::AT_1ST_SIGNAL:
            return "";
        case FaultCode::AT_2ND_SIGNAL:
            return "";
        case FaultCode::AT_3RD_SIGNAL:
            return "";
        case FaultCode::AT_4TH_SIGNAL:
            return "";
        case FaultCode::AT_SHIFT_VALVE_A:
            return "";
        case FaultCode::AT_SHIFT_VALVE_B:
            return "";
        case FaultCode::AT_OVERRUN_CLUTCH_VALVE:
            return "";
        case FaultCode::AT_TORQUE_CONVERTER_VALVE:
            return "";
        case FaultCode::AT_LINE_PRESSURE:
            return "";
        case FaultCode::AT_THROTTLE_POSITION_SENSOR:
            return "";
        case FaultCode::AT_ENGINE_SPEED:
            return "";
        case FaultCode::AT_TEMPERATURE_SENSOR:
            return "";
        default:
            std::string error = cmn::pformat("Unknown fault code: %02x", code);
            throw std::invalid_argument(error);
    }
}


}
