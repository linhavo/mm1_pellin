/*
 * AudioPlatform.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIOPLATFORM_H_
#define AUDIOPLATFORM_H_
/*!
 * @file AudioPlatform.h
 * @brief Platform specific definitions
 *
 * This file contains specifications for types that are specific
 * to the platform used
 */

#include "PlatformDefs.h"
#ifdef _WIN32
#include "WinMMDevice.h"
namespace iimaudio {
typedef iimaudio::WinMMDevice PlatformDevice;
}
#else
#ifdef __linux__
#include "AlsaDevice.h"
namespace iimaudio {
typedef iimaudio::AlsaDevice PlatformDevice;
}
#else
#error Unsupported platform
#endif
#endif



namespace iimaudio {

//typedef PlatformDevice::audio_id_t audio_id;

}
#endif /* AUDIOPLATFORM_H_ */
