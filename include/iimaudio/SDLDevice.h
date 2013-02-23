/*
 * SDLDevice.h
 *
 *  Created on: 23.2.2013
 *      Author: neneko
 */

#ifndef SDLDEVICE_H_
#define SDLDEVICE_H_

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

namespace iimaudio {

struct RGB {
	uint8_t r:8;
	uint8_t g:8;
	uint8_t b:8;
};
struct sdl_pimpl_t;
class SDLDevice {
public:
	typedef std::vector<RGB> data_type;
	SDLDevice(size_t width, size_t height, const std::string& title = "IIMAudio application");
	~SDLDevice();
	bool start();
	bool stop();
	template<typename T>
	bool update(const std::vector<T>&);
private:
	size_t width_;
	size_t height_;
	const std::string title_;
	std::thread thread_;
	std::mutex thread_mutex_;
	std::mutex surface_mutex_;
	std::mutex data_mutex_;
	std::atomic<bool> finish_;
	std::unique_ptr<sdl_pimpl_t> pimpl_;
	data_type data_;
	bool data_changed_;
	std::atomic<bool> flip_required_;

	void run();
	bool process_events();
	void update_data();
};

template<>
bool SDLDevice::update(const data_type& data) {
	if (finish_) return false;
	std::unique_lock<std::mutex> lock(data_mutex_);
	if (data.size()>data_.size()) data_.resize(data.size());
	std::copy(data.begin(),data.end(),data_.begin());
	data_changed_ = true;
	return true;
}
template<typename T>
bool SDLDevice::update(const std::vector<T>& data) {
	if (finish_) return false;
	std::unique_lock<std::mutex> lock(data_mutex_);
	std::copy(reinterpret_cast<data_type::pointer>(&data[0]),
			reinterpret_cast<data_type::pointer>(&data[0])+data.size()*sizeof(T)/sizeof(RGB),
			&data_[0]);
	data_changed_ = true;
	return true;
}

}


#endif /* SDLDEVICE_H_ */

