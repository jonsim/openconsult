#ifndef OPENCONSULT_LIB_CONSULT_ENGINE_PARAMETERS_INTERNAL
#define OPENCONSULT_LIB_CONSULT_ENGINE_PARAMETERS_INTERNAL

#include "common.h"
#include "consult_engine_parameters.h"

#include <cstdint>
#include <vector>

namespace openconsult {


/**
 * @brief Retrieves the command byte sequence necessary to query a given
 *      \c EngineParameter from the ECU. Some parameters may have longer
 *      sequences than others.
 *
 * @param parameter The \c EngineParameter to look-up.
 * @return The byte sequence to query the parameter.
 * @throws std::invalid_argument if \c parameter is not valid.
 */
std::vector<uint8_t> engineParameterCommand(EngineParameter parameter);

/**
 * @brief Decodes a byte sequence, as returned when querying the ECU, into a
 *      real value for a particular \c EngineParameter . Some parameters may
 *      require more bytes to decode than others.
 *
 * @param parameter The \c EngineParameter to decode the byte sequence as.
 * @param data A range of bytes (uint8_t) to decode. This range will be advanced
 *      by the number of bytes consumed by the decode.
 * @return Parameter value, expressed as a double, in the unit described by the
 *      parameter.
 * @throws std::invalid_argument if \c parameter is not valid, or if the \c data
 *      range is too short to decode the requested parameter from.
 */
double engineParameterDecode(EngineParameter parameter,
                             cmn::range<std::vector<uint8_t>::const_iterator>& data);


}

#endif