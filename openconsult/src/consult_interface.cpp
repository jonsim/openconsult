#include <sstream>
#include "common.h"
#include "consult_interface.h"

std::string ECUPartNumber::toJSON() const {
    return cmn::pformat("{\n"
                        "  \"part_number\": \"%s\"\n"
                        "}",
                        part_number.c_str());
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
