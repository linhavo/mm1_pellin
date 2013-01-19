/*
 * iimaudio.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef IIMAUDIO_H_
#define IIMAUDIO_H_

#include "iimaudio/AudioTypes.h"
#include "iimaudio/AudioDevice.h"
namespace iimaudio {
typedef AudioDevice<iimaudio::action_type_t::action_capture> CaptureDevice;
typedef AudioDevice<iimaudio::action_type_t::action_playback> PlaybackDevice;
}
#endif /* IIMAUDIO_H_ */
