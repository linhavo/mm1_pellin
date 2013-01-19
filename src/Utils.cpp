/*
 * Utils.cpp
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */
#include "iimaudio/Utils.h"

namespace iimaudio {
Log logger(std::cerr);

log_level Log::mode =
#ifdef NDEBUG
		log_level::info;
#else
		log_level::debug;
#endif

}



