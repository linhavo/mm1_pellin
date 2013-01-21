/**
 * @file 	iimaudio.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines typedefs for simplified use of the library.
 */
#ifndef IIMAUDIO_H_
#define IIMAUDIO_H_

#include "iimaudio/AudioTypes.h"
#include "iimaudio/AudioDevice.h"
#include "iimaudio/AudioPolicies.h"
namespace iimaudio {
typedef AudioDevice<iimaudio::action_type_t::action_capture,SingleThreaded> CaptureDevice;
typedef AudioDevice<iimaudio::action_type_t::action_playback,SingleThreaded> PlaybackDevice;
typedef AudioDevice<iimaudio::action_type_t::action_capture,MultiThreaded> MultithreadedCaptureDevice;
typedef AudioDevice<iimaudio::action_type_t::action_playback,MultiThreaded> MultithreadedPlaybackDevice;

}
#endif /* IIMAUDIO_H_ */
