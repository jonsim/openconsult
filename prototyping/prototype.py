#!/usr/bin/env python3
import argparse
import enum
import json
import serial
import sys

dtc_codes = {
    11: ('Crankshaft position sensor signal circuit',
        '1-degree (POS) signal or 120-degree (REF) signal is not input for predetermined time '
        'while the engine is running. Abnormal correlation is detected between 1-degree (POS) '
        'signal and 120-degree (REF) sianal.'),
    12: ('Mass air flow sensor signal circuit',
        'Mass air flow sensor output voltage is 4.9V or greater for predetermined time when ignition '
        'switch is turned from OFF to ON, or after the engine is stalled. Mass air flow sensor output voltage is less than 0.3V for predetermined time while the engine is running.'),
    13: ('Engine coolant temperature sensor signal circuit',
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

class RegisterParameters(enum.IntEnum):
    # For engines with a single cylinder bank, use the LH_ parameter
    # variants.
    ENGINE_RPM_MSB = 0x00
    ENGINE_RPM_LSB = 0x01
    LH_MAF_VOLTAGE_MSB = 0x04
    LH_MAF_VOLTAGE_LSB = 0x05
    RH_MAF_VOLTAGE_MSB = 0x06
    RH_MAF_VOLTAGE_LSB = 0x07
    COOLANT_TEMP = 0x08
    LH_O2_SENSOR_VOLTAGE = 0x09
    RH_O2_SENSOR_VOLTAGE = 0x0A
    VEHICLE_SPEED = 0x0B
    BATTERY_VOLTAGE = 0x0C
    THROTTLE_POSITION = 0x0D
    FUEL_TEMP = 0x0F
    INTAKE_AIR_TEMP = 0x11
    EXHAUST_GAS_TEMP = 0x12
    DIGITAL_BIT_REGISTER1 = 0x13
    LH_INJECTION_TIMING_MSB = 0x14
    LH_INJECTION_TIMING_LSB = 0x15
    IGNITION_TIMING = 0x16
    AAC_VALVE = 0x17
    LH_AF_ALPHA = 0x1A
    RH_AF_ALPHA = 0x1B
    LH_AF_ALPHA_SELFLEARN = 0x1C
    RH_AF_ALPHA_SELFLEARN = 0x1D
    DIGITAL_BIT_REGISTER2 = 0x1E
    DIGITAL_BIT_REGISTER3 = 0x1F
    MR_FC_MNT = 0x21
    RH_INJECTION_TIMING_MSB = 0x22
    RH_INJECTION_TIMING_LSB = 0x23
    WASTE_GATE_SOLENOID = 0x28
    TURBO_BOOST_SENSOR = 0x29
    ENGINE_MOUNT = 0x2A
    POSITION_COUNTER = 0x2E
    PURGE_CONTROL_VALVE = 0x25
    TANK_FUEL_TEMP = 0x26
    FPCM_DR_VOLTAGE = 0x27
    FUEL_GAUGE_VOLTAGE = 0x2F



class SerialDeviceLog:
    class EntryType(enum.Enum):
        READ = enum.auto()
        WRITE = enum.auto()
    EntryType.READ.log_prefix = 'R: '
    EntryType.WRITE.log_prefix = 'W: '

    def __init__(self, path):
        self._path = path
        self._last_entry_type = None

    def __enter__(self):
        self._file = open(self._path, 'w')

    def __exit__(self, exc_type, exc_value, traceback):
        self._file.close()

    def log_read(self, data: bytes):
        if self._last_entry_type != self.EntryType.READ:
            self._file.write('\n' + self.EntryType.READ.log_prefix)
            self._last_entry_type = self.EntryType.READ
        self._file.write(data.hex())

    def log_write(self, data: bytes):
        if self._last_entry_type != self.EntryType.WRITE:
            self._file.write('\n' + self.EntryType.WRITE.log_prefix)
            self._last_entry_type = self.EntryType.WRITE
        self._file.write(data.hex())


class ByteInterface:
    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_value, traceback):
        pass

    def read(self, size: int = 1) -> bytes:
        raise NotImplementedError

    def write(self, data: bytes) -> None:
        raise NotImplementedError


class SerialDevice(ByteInterface):
    def __init__(self, device: str, log: SerialDeviceLog = None):
        self._port = serial.Serial(device, 9600, timeout=None)
        self._log = log

    def __enter__(self):
        if self._log:
            self._log.__enter__()

    def __exit__(self, exc_type, exc_value, traceback):
        if self._log:
            self._log.__exit__()

    def read(self, size: int = 1) -> bytes:
        data = self._port.read(size)
        if self._log:
            self._log.log_read(data)
        return data

    def write(self, data: bytes) -> None:
        self._port.write(data)
        if self._log:
            self._log.log_write(data)


class SerialDeviceReplay(ByteInterface):
    EntryType = SerialDeviceLog.EntryType

    def __init__(self, path, wrap_reads = False, wrap_writes = False):
        self._path = path
        self._wrap_reads = wrap_reads
        self._wrap_writes = wrap_writes
        self._read_buffer = ''
        self._write_buffer = ''
        self._read_cursor = 0
        self._write_cursor = 0

    def __enter__(self):
        self._file = open(self._path, 'r')

    def __exit__(self, exc_type, exc_value, traceback):
        self._file.close()

    def _fill_read_buffer(self):
        assert not self._read_buffer
        self._file.seek(self._read_cursor)
        line = self._file.readline().strip()
        while line:
            if line.startswith(self.EntryType.READ.log_prefix):
                self._read_buffer = line[len(self.EntryType.READ.log_prefix):]
                self._read_cursor = self._file.tell()
                break
            line = self._file.readline().strip()
        else:
            if self._wrap_reads:
                self._read_cursor = 0
                self._fill_read_buffer()
            else:
                raise TimeoutError('No more read data in replay')

    def _fill_write_buffer(self):
        assert not self._write_buffer
        self._file.seek(self._write_cursor)
        line = self._file.readline().strip()
        while line:
            if line.startswith(self.EntryType.WRITE.log_prefix):
                self._write_buffer = line[len(self.EntryType.WRITE.log_prefix):]
                self._write_cursor = self._file.tell()
                break
            line = self._file.readline().strip()
        else:
            if self._wrap_writes:
                self._write_cursor = 0
                self._fill_write_buffer()
            else:
                raise TimeoutError('No more write data in replay')
        # Advance the read cursor to the new write cursor so any subsequent
        # reads following the right return the correct response.
        self._read_cursor = self._write_cursor

    def read(self, bytes_size: int = 1) -> bytes:
        str_size = bytes_size * 2
        if not self._read_buffer:
            self._fill_read_buffer()
        if len(self._read_buffer) >= str_size:
            data_bytes = bytes.fromhex(self._read_buffer[:str_size])
            self._read_buffer = self._read_buffer[str_size:]
        else:
            data_bytes = bytes.fromhex(self._read_buffer)
            self._read_buffer = ''
            data_bytes += self.read(bytes_size - len(data_bytes))
        return data_bytes

    def write(self, data_bytes: bytes) -> None:
        data_str = data_bytes.hex()
        while True:
            # TODO: would be nice if we didn't infinitely loop if the log can
            # never fulfil the write.
            # Is the write buffer long enough to contain the requested write
            # data? If not discard and refill.
            if len(self._write_buffer) < len(data_str):
                self._write_buffer = ''
                self._fill_write_buffer()
                continue
            # Is our write data in the write buffer? If not discard and refill.
            pos = self._write_buffer.find(data_str)
            if pos < 0:
                self._write_buffer = ''
                self._fill_write_buffer()
                continue
            # If it is, drain up to the write data.
            self._write_buffer = self._write_buffer[pos + len(data_str):]
            break


class ConsultResponse:
    def __init__(self, frame_fetcher, terminator):
        self._frame_fetcher = frame_fetcher
        self._terminator = terminator

    def __enter__(self):
        yield from self._frame_fetcher()

    def __exit__(self, exc_type, exc_value, traceback):
        if self._terminator:
            self._terminator()


class ConsultInterface:
    def __init__(self, device: ByteInterface, verify: bool = True):
        self._device = device
        self._verify = verify
        self._connect()

    def _connect(self) -> None:
        self._device.write(b'\xFF\xFF\xEF')
        while self._device.read() != b'\x10':
            pass

    def _read_frame(self):
        response = self._device.read(2)
        assert response[0] == 0xFF, f'Frame header did not start with start byte: {response}'
        data_bytes = response[1]    # Data bytes to follow
        return self._device.read(data_bytes)

    def _yield_frames(self):
        while True:
            yield self._read_frame()

    def _halt(self):
        self._device.write(b'\x30')
        response = self._device.read()
        while response != b'\xCF':
            # There's another frame coming - read it then look for another
            # stop-ack afterwards.
            assert response == b'\xFF', f'Frame header did not start with start byte: {response}'
            data_bytes = self._device.read()[0]
            self._device.read(data_bytes)
            response = self._device.read()

    def execute(self, request: bytes, command_width: int = 1, data_width: int = -1, verify: bool = None) -> ConsultResponse:
        if verify is None:
            verify = self._verify
        # Calculate expected response.
        if verify:
            if command_width < 0:
                command_width = len(request)
            if data_width < 0:
                data_width = len(request) - command_width
            is_command_byte = command_width > 0
            parsed_command_width = 0
            parsed_data_width = 0
            expected_response = []
            for byte in request:
                if is_command_byte:
                    expected_response.append(255 - byte)
                    parsed_command_width += 1
                    if parsed_command_width >= command_width:
                        is_command_byte = data_width == 0
                        parsed_command_width = 0
                else:
                    expected_response.append(byte)
                    parsed_data_width += 1
                    if parsed_data_width >= data_width:
                        is_command_byte = command_width > 0
                        parsed_data_width = 0
            expected_response = bytes(expected_response)
            expected_response_len = len(expected_response)
        else:
            expected_response_len = len(request)

        # Transmit the request and check the response matches.
        self._device.write(request)
        response = self._device.read(expected_response_len)
        if verify:
            assert response == expected_response, \
                    f'ECU\'s response ({response.hex()}) was incorrect (should be '\
                    f'{expected_response.hex()} for request {request.hex()}).'

        # Send go-ahead and return a frame reader.
        self._device.write(b'\xF0')
        return ConsultResponse(self._yield_frames, self._halt)



def port_read_ecu_part_number(interface: ConsultInterface):
    with interface.execute(b'\xD0') as frames:
        frame = next(frames)

        x1 = frame[2]
        x2 = frame[3]
        y1 = frame[19]
        y2 = frame[20]
        y3 = frame[21]
        return {
            'part_number': f'{x1:X}{x2:X} 23710-{y1:X}{y2:X}{y3:X}',
        }

def port_read_fault_codes(interface: ConsultInterface):
    with interface.execute(b'\xD1') as frames:
        frame = next(frames)

        assert len(frame) % 2 == 0
        codes = []
        for code, starts in zip(frame[0::2], frame[1::2]):
            name, desc = dtc_codes[code]
            codes.append({
                'code': code,
                'name': name,
                'description': desc,
                'starts_since_observed': starts,
            })
        return codes

def port_read_registers(interface: ConsultInterface):
    registers = (RegisterParameters.ENGINE_RPM_MSB,
                 RegisterParameters.ENGINE_RPM_LSB,
                 RegisterParameters.VEHICLE_SPEED,
                 RegisterParameters.BATTERY_VOLTAGE)
    command = bytes(sum(zip([0x5A] * len(registers), registers), tuple()))

    with interface.execute(command, data_width=1) as frames:
        frame = next(frames)
        assert len(frame) == len(registers), f'Unexpected frame data {frame}'
        engine_rpm = int(((frame[0] << 8) + frame[1]) * 12.5)
        vehicle_speed_kph = frame[2] * 2
        battery_voltage_v = (frame[3] * 80) / 1000
        return {
            'engine_speed_rpm': engine_rpm,
            'vehicle_speed_kmph': vehicle_speed_kph,
            'battery_v': battery_voltage_v,
        }

def port_capability_scan(interface: ConsultInterface):
    # For now this is only going to work if command verification is disabled.
    capability_matrix = {}
    for register in RegisterParameters:
        try:
            with interface.execute(b'\x5A' + bytes(register), data_width=1, verify=False):
                capability_matrix[register] = True
        except:
            capability_matrix[register] = False
    return {str(r.name): v for r, v in capability_matrix.items()}

def main():
    if sys.version_info < (3,5):
        sys.exit('This script requires Python 3.5 or higher.\n')

    parser = argparse.ArgumentParser(prog = 'open_consult_test')
    parser.add_argument('device',
                        type=str,
                        default='/dev/ttyUSB0',
                        help='Name of the serial device to communicate with.')
    parser.add_argument('--log',
                        type=str,
                        default=None,
                        help='Path to a file to dump all received data to.')
    parser.add_argument('--replay',
                        action='store_true',
                        help='Interpret the serial device as a previous log file and replay it.')
    parser.add_argument('--disable-command-verification',
                        action='store_true',
                        help='Disable verification of written commands. This will allow some '
                             'erroring commands to complete (albeit probably returning the '
                             'wrong data). Generally only a good idea for debugging.')
    args = parser.parse_args()

    # If asked to replay, do so.
    if args.replay:
        device = SerialDeviceReplay(args.device)
    else:
        # If asked to log, open a file to do so.
        log = SerialDeviceLog(args.log) if args.log else None
        device = SerialDevice(args.device, log)

    with device:
        interface = ConsultInterface(device, not args.disable_command_verification)

        # Read and print the ECU part number.
        fault_codes = port_read_ecu_part_number(interface)
        print('ECU PART NUMBER\n'
              '===============\n' +
              json.dumps(fault_codes, indent=2))

        # Read and print the fault codes.
        fault_codes = port_read_fault_codes(interface)
        print('\n'
              'FAULT CODES\n'
              '===========\n' +
              json.dumps(fault_codes, indent=2))

        # Read and print some register values.
        registers = port_read_registers(interface)
        print('\n'
              'REGISTERS\n'
              '=========\n' +
              json.dumps(registers, indent=2))

        # Attempt a capability scan.
        capabilities = port_capability_scan(interface)
        print('\n'
              'CAPABILITY_MATRIX\n'
              '=================\n' +
              json.dumps(capabilities, indent=2))


if __name__ == '__main__':
    main()
