#!/usr/bin/env python3
import argparse
import json
import time
import serial

dtc_codes = {
    11: ('Crankshaft position sensor signal circuit',
        '1-degree (POS) signal or 120-degree (REF) signal is not input for predetermined time '
        'while the engine is running. Abnormal correlation is detected between 1-degree (POS) '
        'signal and 120-degree (REF) sianal.'),
    12: ('Mass air flow sensor signal circuit',
        'Mass air flow sensor output voltage is 4.9V or greater for predetermined time when ignition '
        'switch is turned from OFF to ON, or after the engine is stalled. Mass air flow sensor output voltage is less than 0.3V for predetermined time while the engine is running.'),
    13: ('Engine cooland temperature sensor signal circuit',
        'Engine coolant temperature sensor output voltage is approx. 4.8V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time.'),
    14: ('Vehicle speed sensor signal circuit',
        'No vehicle speed signal is input for predetermined time while the vehicle is being driven after warm up.'),
    16: ('Motor throttle switch signal circuit [RB25DET]',
        'Abnormal correlation is detected between input voltages from the throttle motor sensor and from the motor throttle switch for predetermined time.'),
    17: ('ABS-TCS control unit circuit [RB25DET]',
        'Throttle control unit detects malfunction in the system. (Open throttle sensor harness, etc.). TCS/ABS control unit detects malfunction.'),
    21: ('Ignition signal circuit',
        'No consecutive ignition signal while the engine is running.'),
    22: ('Fuel Pump', None),
    23: ('Idle Switch', None),
    24: ('Throttle Valve Switch', None),
    25: ('Idle Speed Control Valve', None),
    26: ('Turbo pressure sensor signal circuit [RB25DET]',
        'Turbo pressure sensor output voltage is approx. 4.8V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time.'),
    28: ('Overheat',
        'Engine coolant temperature sensor output voltage is approx. 0.35V or less (sensor normal) for predetermined time.'),
    31: ('ECM', None),
    32: ('EGR Function', None),
    33: ('Heated oxygen sensor signal circuit',
        'Heated oxygen sensor output voltage is approx. 0.2V or greater and less than approx. 0.4V for predetermined time while the vehicle is being driven after warm up. Heated oxygen sensor output voltage is approx. 2V or greater for predetermined time.'),
    34: ('Knock sensor signal circuit',
        'At least one knock sensor indicates the output voltage of approx. 4V or greater (open circuit) or less than approx. 1V (short circuit).'),
    35: ('Exhaust Gas Temperature Sensor', None),
    36: ('EGR Control-Back Pressure Transducer', None),
    37: ('Knock Sensor', None),
    38: ('Right hand bank Closed Loop (B2)', None),
    41: ('Intake Air Temperature Sensor', None),
    42: ('Fuel Temperature Sensor', None),
    43: ('Throttle position sensor signal circuit',
        'Throttle position sensor output voltage is approx. 4.7V or greater (open circuit) or less than 0.06V (short circuit) for predetermined time while park/neutral position switch is OFF and vehicle speed is 4 km/h or higher.'),
    44: ('ABS-TCS communication circuit [RB25DET]',
        'Malfunction (open/short circuit, etc.) is detected in multiplex communication line between engine and TCS/ABS.'),
    45: ('Injector Leak', None),
    46: ('Throttle motor sensor signal circuit [RB25DET]',
        'Throttle motor sensor input voltage is approx. 4.8V or greater (open circuit) or less than 0.3V (short circuit) for predetermined time.'),
    47: ('Crankshaft Position Sensor', None),
    51: ('Injector Circuit', None),
    53: ('Oxygen Sensor', None),
    54: ('A/T communication circuit',
        'Malfunction is detected in PA communication circuit in ECM(-TCM). [RB25DE (L/B), RB25DET]. Malfunction (open circuit, short circuit, etc.) is detected in multiplex communication line between ECM and TCM. [RB25DE].'),
    55: ('No malfunction',
        'No malfunction is detected in any monitored circuit.'),
    63: ('No. 6 Cylinder Misfire', None),
    64: ('No. 5 Cylinder Misfire', None),
    65: ('No. 4 Cylinder Misfire', None),
    66: ('No. 3 Cylinder Misfire', None),
    67: ('No. 2 Cylinder Misfire', None),
    68: ('No. 1 Cylinder Misfire', None),
    71: ('Random Misfire', None),
    72: ('TWC Function right hand bank', None),
    76: ('Fuel Injection System Function right hand bank', None),
    77: ('Rear Heated Oxygen Sensor Circuit', None),
    82: ('Crankshaft Position Sensor', None),
    84: ('Automatic Transmission Diagnostic Communication Line', None),
    87: ('Canister Control Solenoid Valve Circuit', None),
    91: ('Front Heated Oxygen Sensor Heater Circuit right hand bank', None),
    94: ('TCC Solenoid Valve', None),
    95: ('Crank Position Cog', None),
    98: ('Engine Coolant Temperature Sensor', None),
    101: ('Front Heated Oxygen Sensor Heater Circuit right hand bank', None),
    103: ('Park/Neutral Position Switch Circuit', None),
    105: ('EGR and EGR Canister Control Solenoid Valve Circuit', None),
    108: ('Canister Purge Control Valve Circuit', None),
    111: ('Inhibitor Switch', None),
    112: ('Automatic Transmission Speed Sensor', None),
    113: ('Automatic Transmission 1st Signal', None),
    114: ('Automatic Transmission 2nd Signal', None),
    115: ('Automatic Transmission 3rd Signal', None),
    116: ('Automatic Transmission 4th Signal', None),
    118: ('Automatic Transmission Shift Solenoid Valve A', None),
    121: ('Automatic Transmission Shift Solenoid Valve B', None),
    123: ('Automatic Transmission Overrun Clutch Solenoid Valve', None),
    124: ('Automatic Transmission TOR Conv Solenoid Valve', None),
    125: ('Automatic Transmission Line Pressure Solenoid', None),
    126: ('Automatic Transmission Throttle Position Sensor', None),
    127: ('Automatic Transmission Engine Speed Signal', None),
    128: ('Automatic Transmission Fluid Temperature Sensor', None),
}



def read_and_log(port, size=1, log_file=None):
    response = port.read(size)
    if log_file:
        log_file.write(response)
    return response

def read_until_stop(port, log_file=None):
    b = None
    while b != b'\xCF': # Wait until stop acknowledgement is received.
        b = read_and_log(port, 1, log_file)

def port_initialise(port, log_file = None):
    connected = False
    while not connected:
        port.flushInput()
        port.write(b'\xFF\xFF\xEF')
        time.sleep(2)
        response = read_and_log(port, 1, log_file)
        connected = response == b'\x10'
    # Discard anything left in the buffer (hopefully nothing)...
    #port.reset_input_buffer()

def port_read_ecu_part_number(port, log_file = None):
    def parse_response(data):
        x1 = ord(data[2])
        x2 = ord(data[3])
        y1 = ord(data[19])
        y2 = ord(data[20])
        y3 = ord(data[21])
        return {
            'part_number': f'{x1:X}{x2:X} 23710-{y1:X}{y2:X}{y3:X}',
        }
    # Send part number command.
    port.write(b'\xD0\xF0')
    # Read response.
    data = read_and_log(port, 22, log_file)
    assert data[0] == b'\x2F' # Inverted command byte
    assert data[1] == b'\xFF' # Frame start byte
    # Send stop command.
    port.write(b'\x30')
    read_until_stop(port, log_file)
    # Discard anything left in the buffer (hopefully nothing)...
    #port.reset_input_buffer()
    return parse_response(data)

def port_read_fault_codes(port, log_file = None):
    def parse_response(data):
        assert len(data) % 2 == 0
        codes = []
        for code, count in zip(data[0::2], data[1::2]):
            name, desc = dtc_codes[ord(code)]
            count = ord(count)
            codes.append({
                'name': name,
                'description': desc,
                'count': count,
            })
        return codes

    # Send DTC command.
    port.write(b'\xD1\xF0')
    # Read response.
    data = read_and_log(port, 3, log_file)
    assert data[0] == b'\x2E' # Inverted command byte
    assert data[1] == b'\xFF' # Frame start byte
    data_bytes = ord(data[2]) # Data bytes to follow
    data = read_and_log(port, data_bytes, log_file)
    # Send stop command.
    port.write(b'\x30')
    read_until_stop(port, log_file)
    # Discard anything left in the buffer (hopefully nothing)...
    #port.reset_input_buffer()
    return parse_response(data)

def port_read_registers(port, log_file = None):
    return {}

def main():
    parser = argparse.ArgumentParser(prog = 'open_consult_test')
    parser.add_argument('port',
                        type=str,
                        default='/dev/ttyUSB0',
                        help='Name of the serial device to communicate with.')
    parser.add_argument('--log',
                        type=str,
                        default=None,
                        help='Path to a file to dump all received data to.')
    args = parser.parse_args()

    # If asked to log, open a file to do so.
    if args.log:
        log_file = open(args.log, 'wb')
    else:
        log_file = None

    try:
        # Connect to the specified serial device.
        port = serial.Serial(args.port, 9600, timeout=None)

        # Run the initialisation sequence and wait until the connection is accepted.
        port_initialise(port, log_file)

        # Read and print the ECU part number.
        fault_codes = port_read_ecu_part_number(port, log_file)
        print('ECU PART NUMBER\n'
            '===============')
        json.dumps(fault_codes)

        # Read and print the fault codes.
        fault_codes = port_read_fault_codes(port, log_file)
        print('\n'
            'FAULT CODES\n'
            '===========')
        json.dumps(fault_codes)

        # Read and print some register values.
        registers = port_read_registers(port, log_file)
        print('\n'
            'REGISTERS\n'
            '=========')
        json.dumps(registers)
    except:
        if log_file:
            log_file.close()
        raise


if __name__ == '__main__':
    main()
