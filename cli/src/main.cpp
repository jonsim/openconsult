#include "openconsult/src/common.h"
#include "openconsult/src/consult_interface.h"
#include "openconsult/src/log_replay.h"
#include "openconsult/src/serial.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/usage_config.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace openconsult;

#define APP_NAME "openconsult_cli"
#define APP_VERSION "0.1.0"
#define APP_DESCRIPTION "Command line utility for reading from a Consult device."
// Keep USAGE to < 100 characters per line, including the newline.
#define APP_USAGE "usage: " APP_NAME " [--help] [--version] [--log path] [--replay]\n"\
              "           [--replay_wrap] device"

ABSL_FLAG(std::string, log, "",
          "Path to log all Consult transactions to. This log may be subsequently "
          "'replayed' using the --replay flag.");
ABSL_FLAG(bool, replay, false,
          "Interpret the passed device as a log to replay transactions from.");
ABSL_FLAG(bool, replay_wrap, false,
          "When replaying a log, wrap at the end of the log.");

void reportUsageError(std::string error) {
    std::cerr << APP_USAGE << "\n";
    std::cerr << "ERROR: " << error << "\n";
    std::exit(2);
}

int main(int argc, char** argv) {
    // Configure Abseil flags.
    absl::FlagsUsageConfig flag_config;
    flag_config.version_string = [](){ return APP_NAME " " APP_VERSION "\n"; };
    absl::SetFlagsUsageConfig(flag_config);
    absl::SetProgramUsageMessage(APP_DESCRIPTION "\n" APP_USAGE);

    // Parse command line.
    auto positional_args = absl::ParseCommandLine(argc, argv);
    // Validate command line.
    if (positional_args.size() < 2) {
        reportUsageError("The following arguments are required: device");
    } else if (positional_args.size() > 2) {
        reportUsageError("Too many positional arguments supplied");
    }

    std::string device_id = positional_args[1];
    std::cout << device_id << "\n";
    bool replay = absl::GetFlag(FLAGS_replay);

    std::unique_ptr<ByteInterface> device;
    if (replay) {
        bool wrap = absl::GetFlag(FLAGS_replay_wrap);
        std::ifstream device_file(device_id, std::ios_base::in);
        if (!device_file.good()) {
            reportUsageError(cmn::pformat("Failed to open %s", device_id.c_str()));
        }
        device = std::unique_ptr<ByteInterface>(new LogReplay(device_file, wrap));
    } else {
        std::string log_path = absl::GetFlag(FLAGS_log);
        if (!log_path.empty()) {
            // Construct the log.
        }
        device = std::unique_ptr<ByteInterface>(new SerialPort(device_id, 9600));
    }

    ConsultInterface consult(std::move(device));

    return 0;
}
