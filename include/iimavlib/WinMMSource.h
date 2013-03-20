/**
 * @file 	WinMMSource.h
 *
 * @date 	12.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines WinMM-based source filter
 */

#ifndef WinMMSOURCE_H_
#define WinMMSOURCE_H_

#include "AudioFilter.h"
#include "WinMMDevice.h"
namespace iimavlib {
class EXPORT WinMMSource: public AudioFilter {
public:
	WinMMSource(const audio_params_t& params=audio_params_t(),
				WinMMDevice::audio_id_t id = WinMMDevice::default_device());
	virtual ~WinMMSource();
private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
	virtual audio_params_t do_get_params() const;
	WinMMDevice device_;
};

}
#endif /* WinMMSOURCE_H_ */
