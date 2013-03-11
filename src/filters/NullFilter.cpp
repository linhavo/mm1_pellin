/*
 * NullFilter.cpp
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#include "iimavlib/filters/NullFilter.h"
namespace iimavlib {

NullFilter::NullFilter(const pAudioFilter& child)
:AudioFilter(child)
{

}
NullFilter::~NullFilter()
{

}
error_type_t NullFilter::do_process(audio_buffer_t& /*buffer*/)
{
	return error_type_t::ok;
}
}



