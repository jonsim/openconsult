#ifndef OPENCONSULT_LIB_CONSULT_INTERFACE
#define OPENCONSULT_LIB_CONSULT_INTERFACE

#include "byte_interface.h"
#include "consult_engine_parameters.h"
#include "consult_fault_codes.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace openconsult {


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
 * @brief A response holding metadata about the ECU.
 */
struct ECUMetadata : public ConsultResponse {
    ECUMetadata(const std::vector<uint8_t>& frame);

    std::string toJSON() const override;

    /// @brief The ECU's part number. May contain whitespace and other
    ///     non-alphanumeric characters.
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


/**
 * @brief RAII class for communicating with a Consult device.
 */
class ConsultInterface {
public:
    /**
     * @brief Construct a new \c ConsultInterface for communicating with a
     *      Consult device.
     *
     * @param byte_interface The interface with which to communicate with the
     *      Consult device.
     */
    ConsultInterface(std::unique_ptr<ByteInterface> byte_interface);

    // ConsultInterface is not copyable.
    ConsultInterface(const ConsultInterface&) = delete;
    ConsultInterface& operator=(const ConsultInterface&) = delete;
    // ConsultInterface is movable.
    ConsultInterface(ConsultInterface&&);
    ConsultInterface& operator=(ConsultInterface&&);

    /**
     * @brief Destroy the \c ConsultInterface .
     */
    virtual ~ConsultInterface();

    /**
     * @brief Read identifying information about the ECU.
     *
     * @return ECUMetadata describing the ECU.
     */
    ECUMetadata readECUMetadata();

    /**
     * @brief Read any active fault codes from the ECU.
     *
     * @return FaultCodes describing recently observed fault codes.
     */
    FaultCodes readFaultCodes();

    /**
     * @brief Read the current value of one or more \c EngineParameter s from
     *      the ECU.
     *
     * @param params The \c EngineParameter s to read.
     * @return EngineParameters describing the current value of each of the
     *      requested parameters.
     */
    EngineParameters readEngineParameters(const std::vector<EngineParameter>& params);

    /**
     * @brief Request a stream of the live value of one or more \c
     *      EngineParameter s from the ECU.
     *
     * The returned stream uses this \c ConsultInterface 's underlying
     * connection to retrieve the data. The stream object must live no longer
     * than this interface. While the stream object is alive, no further methods
     * may be called on this interface.
     *
     * @param params The \c EngineParameter s to stream.
     * @return EngineParametersStream object representing the streamed data.
     *      This RAII object will continue streaming data until disposed of, at
     *      which point it will halt the streamed data.
     */
    EngineParametersStream streamEngineParameters(const std::vector<EngineParameter>& params);

private:
    friend class ConsultResponseStream<EngineParameters>;
    class impl;
    std::unique_ptr<impl> pimpl;
};


template <>
class ConsultResponseStream<EngineParameters> {
public:
    ConsultResponseStream(ConsultInterface::impl* pimpl, const std::vector<EngineParameter>& parameters);

    // As the stream uses RAII, it is not copyable.
    ConsultResponseStream(const ConsultResponseStream<EngineParameters>&) = delete;
    ConsultResponseStream<EngineParameters>& operator=(const ConsultResponseStream<EngineParameters>&) = delete;

    // The stream can be moved, however.
    ConsultResponseStream(ConsultResponseStream<EngineParameters>&&);
    ConsultResponseStream<EngineParameters>& operator=(ConsultResponseStream<EngineParameters>&&);

    ~ConsultResponseStream();

    /// @copydoc ConsultResponseStream::getFrame()
    EngineParameters getFrame();

private:
    ConsultInterface::impl* pimpl;
    std::vector<EngineParameter> parameters;
};


}

#endif