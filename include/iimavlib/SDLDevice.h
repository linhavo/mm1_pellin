/**
 * @file 	SDLDevice.h
 *
 * @date 	23.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#ifndef SDLDEVICE_H_
#define SDLDEVICE_H_
#include "PlatformDefs.h"
#include "keys.h"
#include "video_types.h"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include <ostream>
namespace iimavlib {


struct sdl_pimpl_t;
class EXPORT SDLDevice {
public:
	SDLDevice(int width, int height, const std::string& title = "IIMAudio application", bool fullscreen = false);
	virtual ~SDLDevice();
	bool start();
	bool stop();
	bool blit(const video_buffer_t&, rectangle_t position = {0, 0, -1, -1});
	bool is_stopped() const;
	rectangle_t get_size() const { return data_.size; }
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
	video_buffer_t data_;
	bool data_changed_;
	bool fullscreen_;
	std::atomic<bool> flip_required_;

	void run();
	bool process_events();
	void update_data();
	bool key_pressed(const int key, bool pressed);
	bool mouse_moved(const int x, const int y, const int dx, const int dy);
	bool mouse_button(const int key, const bool pressed, const int x, const int y);
	virtual bool do_key_pressed(const int key, bool pressed);
	virtual bool do_mouse_moved(const int x, const int y, const int dx, const int dy);
	virtual bool do_mouse_button(const int key, const bool pressed, const int x, const int y);
};

}


#endif /* SDLDEVICE_H_ */

