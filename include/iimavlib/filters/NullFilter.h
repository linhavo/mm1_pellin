/*
 * NullFilter.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef NULLFILTER_H_
#define NULLFILTER_H_
#include "../AudioFilter.h"
namespace iimavlib {
class EXPORT NullFilter: public AudioFilter {
public:
	NullFilter(const pAudioFilter& child);
	virtual ~NullFilter();
private:
	virtual error_type_t do_process(audio_buffer_t& buffer);
};

}

#endif /* NULLFILTER_H_ */
