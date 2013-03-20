/**
 * @file 	NullFilter.h
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file declares simple dummy filter
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
