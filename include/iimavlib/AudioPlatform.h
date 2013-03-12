/**
 * @file 	AudioPlatform.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines platform specific defaults.
 */

#ifndef AUDIOPLATFORM_H_
#define AUDIOPLATFORM_H_

#include "PlatformDefs.h"
#ifdef _WIN32
#include "WinMMDevice.h"
#include "WinMMSource.h"
#include "WinMMSink.h"
namespace iimavlib {
typedef iimavlib::WinMMDevice PlatformDevice;
typedef iimavlib::WinMMSink PlatformSink;
typedef iimavlib::WinMMSource PlatformSource;
}
#else
#ifdef __linux__
#include "AlsaDevice.h"
#include "AlsaSink.h"
#include "AlsaSource.h"
namespace iimavlib {
typedef iimavlib::AlsaDevice PlatformDevice;
typedef iimavlib::AlsaSink PlatformSink;
typedef iimavlib::AlsaSource PlatformSource;
}
#else
#error Unsupported platform
#endif
#endif



namespace iimavlib {

typedef PlatformDevice::audio_id_t audio_id_t;
typedef PlatformSink DefaultSink;
typedef PlatformSource DefaultSource;

}
#endif /* AUDIOPLATFORM_H_ */
