/**
 * @file 	AudioFilter.h
 *
 * @date 	27.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 * This file defines generic audio filter
 */

#ifndef AUDIOFILTER_H_
#define AUDIOFILTER_H_
#include "AudioTypes.h"
#include "PlatformDefs.h"
#include <memory>
#include <vector>

namespace iimavlib {

/**
 * @brief typedef to use for working with instances of audio filters
 */
typedef EXPORT std::shared_ptr<class AudioFilter> pAudioFilter;

/**
 * @brief Generic audio filter
 *
 * All audio filters should inherit from this class and implement @em do_process method
 */
class EXPORT AudioFilter {
public:
	/**
	 * @brief Constructor
	 * @param child pointer to an instance of child filter.
	 * 	Set it to @em iimavlib::pAudioFilter() when there's no child (i.e. when implementing a source filter)
	 */
	AudioFilter(const pAudioFilter& child);
	/**
	 * @brief Destructor
	 */
	virtual ~AudioFilter();
	/**
	 * @brief Public interface for processing audio samples.
	 *
	 * The method should get a buffer which is correctly allocated and
	 * has at least the @em valid_samples attribute set to correct value.
	 * After the method returns, number of processed samples will be stored in @em valid_samples again.
	 * @note Number of processed samples may be 0 even when error_type_t::ok was returned.
	 *
	 * @param buffer An input/output buffer
	 * @return error_type_t::ok if the processing was successfull, error_type_t::failed otherwise.
	 */
	error_type_t process(audio_buffer_t& buffer);
	/**
	 * @brief Return current params, if the filter (or it's childs) set them.
	 *
	 * The parameters are usually set by the source filter, which gets them
	 * either from user during initialization, or from the input (e.g. an audio file)
	 *
	 * @return Current parameters
	 */
	audio_params_t get_params() const;
private:
	/**
	 * @brief Implementation of buffer processing
	 *
	 * The only method filter inheriting from AudioFilter have to implement.
	 * Source filters should fill up to @em valid_samples of the buffer and set @em valid_samples to the number of processed samples.
	 * Normal filter should process all the @em valid_samples (and keep @em valid_samples) unchanged.
	 * Otherwise data loss would occur.
	 *
	 * @param buffer An input/output buffer
	 * @return error_type_t::ok if the processing was successfull, error_type_t::failed otherwise.
	 */
	virtual error_type_t do_process(audio_buffer_t& buffer) = 0;
	/**
	 * @brief Method providing current parameters
	 *
	 * Implementations of filters should re-implement this method ONLY when they are actually having it's own parameter.
	 * Usually this means that only source filters should implement it.
	 *
	 * @return Current parameters
	 */
	virtual audio_params_t do_get_params() const;
	pAudioFilter child_;
};


}


#endif /* AUDIOFILTER_H_ */
