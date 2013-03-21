/**
 * @file 	NullFilter.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
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



