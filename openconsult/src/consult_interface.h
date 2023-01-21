#ifndef OPENCONSULT_LIB_CONSULT_INTERFACE
#define OPENCONSULT_LIB_CONSULT_INTERFACE

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "byte_interface.h"
#include "consult_engine_parameters.h"
#include "consult_fault_codes.h"


/**
 * @brief A response from a \c ConsultInterface .
 */
class ConsultResponse {
public:
    /**
     * @brief Serialize the response into JSON.
     *
     * @return String representation of the response in JSON format. Not
     *      minimized. May contain newlines and indents.
     */
    virtual std::string toJSON() const = 0;
};


/**
 * @brief A response stream from a \c ConsultInterface .
 *
 * @tparam ResponseType The response object being streamed. Must extend
 *      \c ConsultResponse .
 */
template <class ResponseType>
class ConsultResponseStream {
public:
    /**
     * @brief Blocking call to retrieve a single frame from the stream. Each
     *      call will return a new frame, blocking until it is available.
     *
     * @return The next frame in the stream.
     */
    ResponseType getFrame();
};


/**
 * @brief A response holding the ECU's part number.
 */
struct ECUPartNumber : public ConsultResponse {
    ECUPartNumber(const std::vector<uint8_t>& frame);

    std::string toJSON() const override;

    /// @brief The ECU's part number. May contain whitespace.
    std::string part_number;
};


/**
 * @brief A response holding information about a single observed fault.
 */
struct FaultCodeData : public ConsultResponse {
    FaultCodeData(const std::vector<uint8_t>& frame);

    std::string toJSON() const override;

    /// @brief The \c FaultCode that was observed by the ECU.
    FaultCode fault_code;
    /// @brief The number of times the engine has been started since the fault
    ///     code was last seen.
    uint32_t starts_since_observed;
};


/**
 * @brief A response holding information about all observed fault codes.
 */
struct FaultCodes : public ConsultResponse {
    FaultCodes(const std::vector<uint8_t>& frame);

    std::string toJSON() const override;

    /// @brief Vector of \c FaultCodeData that have been observed.
    std::vector<FaultCodeData> fault_codes;
};


/**
 * @brief A response holding the current value of one or more engine parameters.
 */
struct EngineParameters : public ConsultResponse {
    EngineParameters(const std::vector<EngineParameter>& parameters,
                     const std::vector<uint8_t>& frame);

    std::string toJSON() const override;

    /// @brief Map of \c EngineParameter to their current value.
    std::map<EngineParameter, double> parameters;
};


/**
 * @brief A stream of responses describing the live value of one or more engine
 *      parameters. Each frame contains the same engine parameters.
 */
using EngineParametersStream = ConsultResponseStream<EngineParameters>;

#endif