/**
 * @file 	AudioSink.cpp
 *
 * @date 	11.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */


#include "iimavlib/AudioSink.h"

namespace iimavlib {

AudioSink::AudioSink(const pAudioFilter& child_):AudioFilter(child_),
		running_(false)
{

}
AudioSink::~AudioSink()
{
}
error_type_t AudioSink::run()
{
	running_.store(true);
	return do_run();
}
error_type_t AudioSink::do_process(audio_buffer_t& /*buffer*/)
{
	return error_type_t::ok;
}

void AudioSink::stop()
{
	running_.store(false);
}

bool AudioSink::still_running() const
{
	return running_;
}
void AudioSink::set_buffers(size_t count, size_t size)
{
	do_set_buffers(count, size);
}
void AudioSink::do_set_buffers(size_t /*count*/, size_t /*size*/)
{
}
}

