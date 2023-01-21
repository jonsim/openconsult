#include "consult_interface.h"
#include "common.h"
#include <sstream>

namespace openconsult {


//
// ECUMetadata
//

ECUMetadata::ECUMetadata(const std::vector<uint8_t>& frame) {
    if (frame.size() != 22) {
        throw std::invalid_argument("Invalid ECU part number response");
    }
    part_number = cmn::pformat("%02X%02X 23710-%X%02X%02X",
        frame[2],
        frame[3],
        frame[19],
        frame[20],
        frame[21]);
}

std::string ECUMetadata::toJSON() const {
    return cmn::pformat("{\n"
                        "  \"part_number\": \"%s\"\n"
                        "}",
                        part_number.c_str());
}



//
// FaultCodeData
//

FaultCodeData::FaultCodeData(const std::vector<uint8_t>& frame) {
    if (frame.size() != 2) {
        throw std::invalid_argument("Invalid fault code response");
    }
    fault_code = faultCodeFromId(frame[0]);
    starts_since_observed = frame[1];
}

std::string FaultCodeData::toJSON() const {
    uint32_t id = faultCodeToId(fault_code);
    std::string name = faultCodeName(fault_code);
    std::string desc = faultCodeDescription(fault_code);
    std::stringstream sstream;
    sstream << "{\n"
            << "  \"code\": " << id << ",\n"
            << "  \"name\": \"" << name << "\",\n";
    if (desc.empty()) {
        sstream << "  \"description\": null,\n";
    } else {
        sstream << "  \"description\": \"" << desc << "\",\n";
    }
    sstream << "  \"starts_since_observed\": " << starts_since_observed << "\n"
                << "}";
    return sstream.str();
}



//
// FaultCodes
//

FaultCodes::FaultCodes(const std::vector<uint8_t>& frame) {
    if ((frame.size() & 1) != 0) {
        throw std::invalid_argument("Invalid fault codes response");
    }
    for (std::size_t i = 0; i < frame.size(); i += 2) {
        auto sub_frame = std::vector<uint8_t>{{frame[i], frame[i+1]}};
        fault_codes.push_back(sub_frame);
    }
}

std::string FaultCodes::toJSON() const {
    std::stringstream sstream;
    std::string seperator = "\n";
    sstream << "[";
    for (const auto& data : fault_codes) {
        uint32_t id = faultCodeToId(data.fault_code);
        std::string name = faultCodeName(data.fault_code);
        std::string desc = faultCodeDescription(data.fault_code);
        sstream << seperator
                << "  {\n"
                << "    \"code\": " << id << ",\n"
                << "    \"name\": \"" << name << "\",\n";
        if (desc.empty()) {
            sstream << "    \"description\": null,\n";
        } else {
            sstream << "    \"description\": \"" << desc << "\",\n";
        }
        sstream << "    \"starts_since_observed\": " << data.starts_since_observed << "\n"
                << "  }";
        seperator = ",\n";
    }
    sstream << "\n]";
    return sstream.str();
}



//
// EngineParameters
//

EngineParameters::EngineParameters(const std::vector<EngineParameter>& params,
                                   const std::vector<uint8_t>& frame) {
    if (frame.size() < params.size()) {
        throw std::invalid_argument("Invalid engine parameters response");
    }
    auto range = cmn::make_range(frame);
    for (auto param : params) {
        parameters[param] = engineParameterDecode(param, range);
    }
    if (!range.empty()) {
        throw std::invalid_argument("Invalid engine parameters response");
    }
}

std::string EngineParameters::toJSON() const {
    std::stringstream sstream;
    std::string seperator = "\n";
    sstream << "{";
    for (const auto& parameter : parameters) {
        sstream << seperator
                << "  \"" << engineParameterId(parameter.first)
                << "\": " << std::fixed << std::setprecision(2) << parameter.second;
        seperator = ",\n";
    }
    sstream << "\n}";
    return sstream.str();
}



//
// ConsultInterface::impl
//

struct ConsultInterface::impl {
    impl(std::unique_ptr<ByteInterface> _byte_interface)
            : byte_interface(std::move(_byte_interface)) {
        // Connect to the underlying Consult device.
        byte_interface->write({{0xFF, 0xFF, 0xEF}});
        while (byte_interface->read(1)[0] != 0x10) {
            // Spin.
        }
    }

    // Non-copyable.
    impl(const impl&) = delete;
    impl& operator=(const impl&) = delete;

    // Movable.
    impl(impl&& other)
            : byte_interface(std::move(other.byte_interface)) {
    }
    impl& operator=(impl&& other) {
        byte_interface = std::move(other.byte_interface);
        return *this;
    }

    std::vector<uint8_t> calculateExpectedResponse(const std::vector<uint8_t>& request,
                                                   int command_width = 1, int data_width = -1) {
        if (command_width < 0) {
            command_width = request.size();
        }
        if (data_width < 0) {
            data_width = request.size() - command_width;
        }
        bool is_command_byte = command_width > 0;
        int parsed_command_width = 0;
        int parsed_data_width = 0;
        std::vector<uint8_t> response = request;
        for (uint8_t& byte : response) {
            if (is_command_byte) {
                byte = ~byte;
                if (++parsed_command_width >= command_width) {
                    is_command_byte = data_width == 0;
                    parsed_command_width = 0;
                }
            } else {
                if (++parsed_data_width >= data_width) {
                    is_command_byte = command_width > 0;
                    parsed_data_width = 0;
                }
            }
        }
        return response;
    }

    void execute(const std::vector<uint8_t>& request,
                 int command_width = 1, int data_width = -1, bool verify = true) {
        // Send the request and receive the response.
        if (verify) {
            auto expected_response = calculateExpectedResponse(request, command_width, data_width);
            byte_interface->write(request);
            auto response = byte_interface->read(expected_response.size());
            if (response != expected_response) {
                throw std::runtime_error("Unexpected response received");
            }
        } else {
            byte_interface->write(request);
            byte_interface->read(request.size());
        }
        // Send go-ahead and return a frame reader.
        std::vector<uint8_t> go_ahead{0xF0};
        byte_interface->write(go_ahead);
    }

    std::vector<uint8_t> readFrame() {
        auto response = byte_interface->read(2);
        if (response[0] != 0xFF) {
            throw std::runtime_error("Frame header did not start with start byte");
        }
        std::size_t data_bytes = response[1]; // Data bytes to follow.
        return byte_interface->read(data_bytes);
    }

    void halt() {
        byte_interface->write({0x30});
        auto response = byte_interface->read(1);
        while (response[0] != 0xCF) {
            // There's another frame coming - read it then look for another
            // stop-ack afterwards.
            if (response[0] != 0xFF) {
                throw std::runtime_error("Frame header did not start with start byte");
            }
            std::size_t data_bytes = byte_interface->read(1)[0];
            byte_interface->read(data_bytes);
            response = byte_interface->read(1);
        }
    }

    std::unique_ptr<ByteInterface> byte_interface;
};



//
// EngineParametersStream
//

ConsultResponseStream<EngineParameters>::ConsultResponseStream(ConsultInterface::impl* _pimpl,
        const std::vector<EngineParameter>& _parameters)
        : pimpl(_pimpl)
        , parameters(_parameters) {
}

ConsultResponseStream<EngineParameters>::ConsultResponseStream(ConsultResponseStream<EngineParameters>&& other)
        : pimpl(other.pimpl)
        , parameters(std::move(other.parameters)) {
    other.pimpl = nullptr;
}

ConsultResponseStream<EngineParameters>& ConsultResponseStream<EngineParameters>::operator=(ConsultResponseStream<EngineParameters>&& other) {
    pimpl = other.pimpl;
    parameters = std::move(other.parameters);
    other.pimpl = nullptr;
    return *this;
}

ConsultResponseStream<EngineParameters>::~ConsultResponseStream() {
    if (pimpl) {
        pimpl->halt();
    }
}

EngineParameters ConsultResponseStream<EngineParameters>::getFrame() {
    auto frame = pimpl->readFrame();
    return EngineParameters(parameters, frame);
}



//
// ConsultInterface
//

ConsultInterface::ConsultInterface(std::unique_ptr<ByteInterface> byte_interface)
        : pimpl(new impl(std::move(byte_interface))) {
}

ConsultInterface::ConsultInterface(ConsultInterface&& other)
        : pimpl(std::move(other.pimpl)) {
}

ConsultInterface& ConsultInterface::operator=(ConsultInterface&& other) {
    pimpl = std::move(other.pimpl);
    return *this;
}

ConsultInterface::~ConsultInterface() {
}

ECUMetadata ConsultInterface::readECUMetadata() {
    std::vector<uint8_t> request{0xD0};
    pimpl->execute(request);
    auto frame = pimpl->readFrame();
    pimpl->halt();
    return ECUMetadata(frame);
}

FaultCodes ConsultInterface::readFaultCodes() {
    std::vector<uint8_t> request{0xD1};
    pimpl->execute(request);
    auto frame = pimpl->readFrame();
    pimpl->halt();
    return FaultCodes(frame);
}

EngineParameters ConsultInterface::readEngineParameters(const std::vector<EngineParameter>& params) {
    std::vector<uint8_t> request;
    for (auto param : params) {
        auto command = engineParameterCommand(param);
        request.insert(request.end(), command.begin(), command.end());
    }
    pimpl->execute(request, 1, 1);
    auto frame = pimpl->readFrame();
    pimpl->halt();
    return EngineParameters(params, frame);
}

EngineParametersStream ConsultInterface::streamEngineParameters(const std::vector<EngineParameter>& params) {
    std::vector<uint8_t> request;
    for (auto param : params) {
        auto command = engineParameterCommand(param);
        request.insert(request.end(), command.begin(), command.end());
    }
    pimpl->execute(request, 1, 1);
    return EngineParametersStream(pimpl.get(), params);
}


}
