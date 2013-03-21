/**
 * @file 	AudioSink.h
 *
 * @date 	19.1.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines generic sink filter
 */
#ifndef AUDIOSINK_H_
#define AUDIOSINK_H_
#include "AudioFilter.h"
#include <atomic>
#include <cassert>

namespace iimavlib {
EXPORT typedef std::shared_ptr<class AudioSink> pAudioSink;

class EXPORT AudioSink: public AudioFilter {
public:
	AudioSink(const pAudioFilter& child_);
	virtual ~AudioSink();
	error_type_t run();
	void stop();
	void set_buffers(size_t count, size_t size);
protected:
	bool still_running() const;
private:
	virtual error_type_t do_run() = 0;
	virtual error_type_t do_process(audio_buffer_t& buffer);
	virtual void do_set_buffers(size_t count, size_t size);
	std::atomic<bool> running_;
};

/**
 * Convenience struct to simplify the process of creating filter chain
 * \tparam Src type of the source node
 * \todo This will probably break under VS2012 as it lacks support for variadic templates. Oh well...
 */
template<class Src>
struct filter_chain {
#ifdef MODERN_COMPILER
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
#else
	// Visual Studio 2012 still has no support for variadic templates.
	// So we have to define lots of non-variadic templates....
	// Anyway, this limits VS support to classes with 5 non-child parameters at most
	filter_chain():
		filter_()
	{}
	template<class T1>
	filter_chain(T1&& a1):
		filter_(std::make_shared<Src>(std::forward<T1>(a1)))
	{}

	template<class T1, class T2>
	filter_chain(T1&& a1, T2&& a2):
		filter_(std::make_shared<Src>(std::forward<T1>(a1), std::forward<T2>(a2)))
	{}
	template<class T1, class T2, class T3>
	filter_chain(T1&& a1, T2&& a2, T3&& a3):
		filter_(std::make_shared<Src>(std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3)))
	{}
	template<class T1, class T2, class T3, class T4>
	filter_chain(T1&& a1, T2&& a2, T3&& a3, T4&& a4):
		filter_(std::make_shared<Src>(std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3),
										std::forward<T4>(a4)))
	{}
	template<class T1, class T2, class T3, class T4, class T5>
	filter_chain(T1&& a1, T2&& a2, T3&& a3, T4&& a4, T5&& a5):
		filter_(std::make_shared<Src>(std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3),
										std::forward<T4>(a4),
										std::forward<T5>(a5)))
	{}
	/**
	 * Method for adding new filters into the chain
	 * \tparam T Type of the filter
	 * @param args Argumets to pass to the constructor of class &em T
	 * @return Reference to filter_chain
	 */
	template<class T>
	filter_chain& add()
	{
		assert(filter_);
		filter_.reset(new T (filter_));
		return *this;
	}
	template<class T, class T1>
	filter_chain& add(T1&& a1)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<T1>(a1)));
		return *this;
	}
	template<class T, class T1, class T2>
	filter_chain& add(T1&& a1, T2&& a2)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<T1>(a1), std::forward<T2>(a2)));
		return *this;
	}
	template<class T, class T1, class T2, class T3>
	filter_chain& add(T1&& a1, T2&& a2, T3&& a3)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3)));
		return *this;
	}
	template<class T, class T1, class T2, class T3, class T4>
	filter_chain& add(T1&& a1, T2&& a2, T3&& a3, T4&& a4)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3),
										std::forward<T3>(a4)));
		return *this;
	}
	template<class T, class T1, class T2, class T3, class T4, class T5>
	filter_chain& add(T1&& a1, T2&& a2, T3&& a3, T4&& a4, T5&& a5)
	{
		assert(filter_);
		filter_.reset(new T (filter_, std::forward<T1>(a1),
										std::forward<T2>(a2),
										std::forward<T3>(a3),
										std::forward<T3>(a4),
										std::forward<T3>(a5)));
		return *this;
	}
#endif
	filter_chain(filter_chain&& rhs):filter_(std::move(rhs.filter_)) {}
	filter_chain(filter_chain& rhs):filter_(rhs.filter_) {}
	filter_chain(const filter_chain& rhs):filter_(rhs.filter_) {}
	filter_chain& operator=(const filter_chain& rhs)
	{
		filter_=rhs.filter_;
		return *this;
	}
	~filter_chain() {}
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
