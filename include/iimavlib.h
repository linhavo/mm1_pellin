/**
 * @file 	iimavlib.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines typedefs for simplified use of the library.
 */
#ifndef IIMAUDIO_H_
#define IIMAUDIO_H_

#include "iimavlib/AudioTypes.h"
#include "iimavlib/AudioDevice.h"
#include "iimavlib/AudioPolicies.h"
namespace iimavlib {
typedef AudioDevice<iimavlib::action_type_t::action_capture,SingleThreaded> CaptureDevice;
typedef AudioDevice<iimavlib::action_type_t::action_playback,SingleThreaded> PlaybackDevice;
typedef AudioDevice<iimavlib::action_type_t::action_capture,MultiThreaded> MultithreadedCaptureDevice;
typedef AudioDevice<iimavlib::action_type_t::action_playback,MultiThreaded> MultithreadedPlaybackDevice;

}
#endif /* IIMAUDIO_H_ */
