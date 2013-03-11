/*
 * AudioSink.h
 *
 *  Created on: 11.3.2013
 *      Author: neneko
 */

#ifndef AUDIOSINK_H_
#define AUDIOSINK_H_
#include "AudioFilter.h"
#include <atomic>
#include <cassert>

namespace iimavlib {
typedef std::shared_ptr<class AudioSink> pAudioSink;

class AudioSink: public AudioFilter {
public:
	AudioSink(const pAudioFilter& child_);
	virtual ~AudioSink();
	error_type_t run();
	void stop();
protected:
	bool still_running() const;
private:
	virtual error_type_t do_run() = 0;
	virtual error_type_t do_process(audio_buffer_t& buffer);
	std::atomic<bool> running_;
};

/**
 * Convenience struct to simplify the process of creating filter chain
 * \tparam Src type of the source node
 * \todo This will probably break under VS2012 as it lacks support for variadic templates. Oh well...
 */
template<class Src>
struct filter_chain {

	/**
	 * Constructor for initializing source filter
	 * @param args Argumets to forward to constructor of class @em Src
	 */
	template<class... Args>
	filter_chain(Args&&... args):
		filter_(std::make_shared<Src>(std::forward<Args>(args)...))
	{}

	/**
	 * Method for adding new filters into the chain
	 * \tparam T Type of the filter
	 * @param args Argumets to pass to the constructor of class &em T
	 * @return Reference to filter_chain
	 */
	template<class T, class... Args>
	filter_chain& add(Args&&... args)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<Args>(args)...));
		return *this;
	}
	/**
	 * Conversion operator to the head of filter chain
	 */
	operator pAudioFilter () {return filter_; }

	/**
	 * Method to retrieve sink from the chain.
	 * @return pointer (pAudioSink) to the sink filter, if the head is a sink, empty pAudioSink otherwise.
	 */
	pAudioSink sink() { return std::dynamic_pointer_cast<AudioSink>(filter_);}
private:
	pAudioFilter filter_;
};


}



#endif /* AUDIOSINK_H_ */
