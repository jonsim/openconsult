#ifndef OPENCONSULT_LIB_CONSULT_FAULT_CODES_INTERNAL
#define OPENCONSULT_LIB_CONSULT_FAULT_CODES_INTERNAL

#include "consult_fault_codes.h"

#include <cstdint>

namespace openconsult {


/**
 * @brief Converts a raw ID, as supplied by an ECU, into a \c FaultCode .
 *
 * @param id The ID to convert.
 * @return The \c FaultCode represented by the ID.
 * @throws std::invalid_argument if \c id does not identify a known
 *      \c FaultCode .
 */
FaultCode faultCodeFromId(uint8_t id);

/**
 * @brief Converts a \c FaultCode into a raw ID, as supplied by an ECU.
 *
 * @param code The \c FaultCode to convert.
 * @return The ID used by the ECU to represent \c code .
 */
uint8_t faultCodeToId(FaultCode code);


}

#endif