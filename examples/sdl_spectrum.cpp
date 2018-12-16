/**
 * @file 	sdl_spectrum.cpp
 *
 * @date 	29.4.2016
 * @author 	Roman Berka <berka@fel.cvut.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/AudioFFT.h"
#include "iimavlib/AudioTypes.h"

#include "iimavlib/SDLDevice.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib_high_api.h"
#include "iimavlib/Utils.h"
#include "iimavlib/video_ops.h"

#include "iimavlib/filters/SineMultiply.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>
#include <atomic>
#include <functional>

using namespace iimavlib;
class Spectrum: public AudioFilter {
public:
	Spectrum(const pAudioFilter& child, int width, int height, double time):
			AudioFilter(child),sdl_(width, height),data_(width,height),width_(width),height_(height),
			time_(time),end_(false),changed_(false),last_sample_(0),cache_size_(0)
		{
			const audio_params_t& params = get_params();
			cache_size_ = static_cast<size_t>(time_*convert_rate_to_int(params.rate));
			cache_size_ = static_cast<size_t>(pow(2,ceil(log2(cache_size_)))*2);
			logger[log_level::info] << "Cache size: " << cache_size_;
			sample_cache_.resize(cache_size_);
			barwidth = 40;
			sdl_.start();
			thread_ = std::thread(std::bind(&Spectrum::execute_thread,this));
		}
		~Spectrum()
		{
			sdl_.stop();
			thread_.join();
		}

	private:
		error_type_t do_process(audio_buffer_t& buffer)
		{
			if (end_) return error_type_t::failed;

			update_cache(buffer);
			return error_type_t::ok;
		}
		void execute_thread() {
			logger[log_level::debug] << "Drawing thread started";
			while (!end_) {
				if (changed_) {
						draw_wave();
				}
				if (!sdl_.blit(data_)) {
					logger[log_level::debug] << "Drawing thread finishing";
					end_ = true;
				}
			}
		}

		void update_cache(const audio_buffer_t& buffer) {
			std::unique_lock<std::mutex> lock(mutex_);
			const audio_sample_t *src = &buffer.data[0];
			size_t src_remaining = buffer.valid_samples;
			while (src_remaining) {
				const size_t to_copy = std::min(src_remaining, sample_cache_.size()-last_sample_);
				std::copy_n(src,to_copy,&sample_cache_[0]+last_sample_);
				last_sample_+=to_copy;
				if (last_sample_ >= sample_cache_.size()) last_sample_ = 0;
				src_remaining-=to_copy;
			}
			changed_.store(true);
		}

		void draw_wave() {
			// Max value for int16_t
			const auto max_int16_value = std::numeric_limits<int16_t>::max();
			// Black color
			const rgb_t black(0, 0, 0);

			const float magic_constant = 1.0f / 16384.0f / max_int16_value;
			changed_.store(false);
			data_.clear(black);

			// Array for the coefficients from FFT
			complexarray_t<float> coefficient_array;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				coefficient_array = fft.FFT1D(sample_cache_.begin(), sample_cache_.end());
			}

			// Number of unique coefficients
			const auto unique_coefficients = (coefficient_array.size() + 1) / 2;

			// This loop calculated the heights of displayed bars
			for (int x = 0; x < width_; ++x) {
				// Calculate the index of coefficient to display in this bar
				const size_t coefficient_number = x * unique_coefficients / width_;

				// Get value for the coefficient
				const auto coefficient = std::abs(coefficient_array[coefficient_number]) * magic_constant;

				// And calculate bar height
				auto y = static_cast<int>(height_ * (1.0f - coefficient));

				// Limit the bar height to interval <0, height_ -1>
				y = std::min(height_ - 1, std::max(y, 0));

				// And draw the bar
				draw_bars(x, y);

			}

		}

		void draw_line(int x, int y) {
			iimavlib::draw_line(data_, rectangle_t(x,height_), rectangle_t(x,y), rgb_t(255,0,0));
		}

		void draw_bars(int x, int y) {
			iimavlib::draw_rectangle(data_,rectangle_t(x, y, barwidth/10, height_-y),rgb_t(255,0,0));
		}

		SDLDevice sdl_;
		video_buffer_t data_;
		std::thread thread_;
		std::mutex mutex_;
		std::vector<audio_sample_t> sample_cache_;
		int width_;
		int height_;
		int barwidth;
		double time_;
		std::atomic<bool> end_;
		std::atomic<bool> changed_;
		size_t last_sample_;
		size_t cache_size_;

		AudioFFT<float> fft;
};

int main(int argc, char** argv)
{
	if (argc<2) {
		logger[log_level::fatal] << "Usage: " << argv[0] << " filename.wav [miliseconds]";
		return 1;
	}
	std::string filename = argv[1];
	audio_id_t device_out = PlatformDevice::default_device();
	double time = 50;
	if (argc>2) time = simple_cast<double>(argv[2]);
	if (argc>3) device_out= simple_cast<audio_id_t>(argv[3]);
	auto chain = filter_chain<WaveSource>(filename)
			.add<Spectrum>(800,600,time/1000.0)
			.add<PlatformSink>(device_out)
			.sink();

	chain->run();
}
