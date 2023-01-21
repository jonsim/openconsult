#include <sstream>
#include "common.h"
#include "consult_interface.h"



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
