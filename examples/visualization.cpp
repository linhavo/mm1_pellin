/**
 * @file 	visualization.cpp
 *
 * @date 	20.3.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#include "iimavlib/WaveSource.h"
#include "iimavlib_high_api.h"
#include "iimavlib/Utils.h"

#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>
#include <atomic>

using namespace iimavlib;
class Visualization: public AudioFilter {
public:
	Visualization(const pAudioFilter& child, size_t width, size_t height, double time):
		AudioFilter(child),sdl_(width, height),data_(width*height),width_(width),height_(height),
		time_(time),end_(false),changed_(false),last_sample_(0),cache_size_(0)
	{
		const audio_params_t& params = get_params();
		num_channels_ = params.num_channels;
		cache_size_ = static_cast<size_t>(time_*convert_rate_to_int(params.rate));
		sample_cache_.resize(cache_size_*num_channels_);
		sdl_.start();
		thread_ = std::thread(std::bind(&Visualization::execute_thread,this));
	}
	~Visualization()
	{
		sdl_.stop();
		thread_.join();
	}
private:
	error_type_t do_process(audio_buffer_t& buffer)
	{
		if (end_) return error_type_t::failed;
		// Currently only 16bit signed samples are supported
		if (buffer.params.format != sampling_format_t::format_16bit_signed) {
			return error_type_t::unsupported;
		}

		update_cache(buffer);
		return error_type_t::ok;
	}
	void execute_thread() {
		logger[log_level::debug] << "Drawing thread started";
		while (!end_) {
			if (changed_) {
					draw_wave();
			}
			if (!sdl_.update(data_)) {
				logger[log_level::debug] << "Drawing thread finishing";
				end_ = true;
			}
		}
	}

	void update_cache(const audio_buffer_t& buffer) {
		std::unique_lock<std::mutex> lock(mutex_);
		const int16_t *src = reinterpret_cast<const int16_t*>(&buffer.data[0]);
		size_t src_remaining = buffer.valid_samples*num_channels_;
		while (src_remaining) {
			size_t to_copy = std::min(src_remaining, sample_cache_.size()-last_sample_);
			std::copy_n(src,to_copy,&sample_cache_[0]+last_sample_);
			last_sample_+=to_copy;
			if (last_sample_ >= sample_cache_.size()) last_sample_ = 0;
			src_remaining-=to_copy;
		}
		changed_.store(true);
	}

	void draw_wave() {
		changed_.store(false);
		RGB black = {0,0,0};
		std::fill(data_.begin(),data_.end(),black);
		std::vector<size_t> vals;
		vals.reserve(width_);
		{
			std::unique_lock<std::mutex> lock(mutex_);
			for (size_t x = 0;x < width_; ++x) {
				size_t sample_num = x*cache_size_/width_;
				int16_t sample = sample_cache_[sample_num*num_channels_];
				size_t y = static_cast<size_t>(height_/2 + static_cast<double>(height_)*sample/std::numeric_limits<int16_t>::max()/2);
				y = std::min(height_-1,std::max(y,static_cast<size_t>(0)));
				vals.push_back(y);
			}	
		}
		for (size_t x = 1;x < width_; ++x) {
			draw_line(x-1,vals[x-1],vals[x]);
		}
		
	}

	void draw_line(size_t x, size_t y0, size_t y1) {
		size_t y=y0;
		if (y0==y1) {
			data_[x+y*width_].r = 255;
			data_[x+1+y*width_].r = 255;
		} else if (y0 < y1) {
			for (;y<(y1+y0)/2;++y) data_[x+y*width_].r = 255;
			for (;y<y1;++y) data_[x+1+y*width_].r = 255;
		} else {
			for (;y>(y1+y0)/2;--y) data_[x+y*width_].r = 255;
			for (;y>y1;--y) data_[x+1+y*width_].r = 255;
		}
	}
	SDLDevice sdl_;
	SDLDevice::data_type data_;
	std::thread thread_;
	std::mutex mutex_;
	std::vector<int16_t> sample_cache_;
	size_t width_;
	size_t height_;
	double time_;
	std::atomic<bool> end_;
	std::atomic<bool> changed_;
	size_t last_sample_;
	size_t cache_size_;
	size_t num_channels_;
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
	auto chain = filter_chain<WaveSource>(filename)
			.add<Visualization>(800,600,time/1000.0)
			.add<PlatformSink>(device_out)
			.sink();

	//chain->set_buffers(8,512);
	chain->run();
}

