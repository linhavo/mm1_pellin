/**
 * @file 	sdl_window.cpp
 *
 * @date 	23.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>
#include "iimavlib/Utils.h"
#include "iimavlib/video_ops.h"
#include <complex>


namespace {
iimavlib::rgb_t iter_color(int i, int iterations)
{
	const iimavlib::rgb_t white (255,255,255);
	const float val = 1 - (i+1.0f) / iterations;
	return white * val * val;
}

/*!
 * Computes Mandelbrot fractal (using @em iteration iterations)
 * @param data video buffer to render the fractal to
 * @param iterations Number of iterations to perform for each point
 */
void mandelbrot(iimavlib::video_buffer_t& data, int iterations)
{
	for (int line = 0; line < data.size.height; ++line) {
		for (int row = 0; row < data.size.width; ++row) {
			const std::complex<float> z0 (2.0f * row/data.size.width - 1.5f, 2.0f * line/data.size.height - 1.0f);
			std::complex<float> z = z0;
			for (int i =0;i<iterations;++i) {
				z = z*z + z0;
				if (std::abs(z) > 2.0) {
					data(row, line) = iter_color(i, iterations);
					break;
				}
			}
			if (std::abs(z) < 2) data(row, line) = iimavlib::rgb_t(0,0,0);
		}
	}
}



}

int main()
{
	iimavlib::SDLDevice sdl(800, 600, "HEY!");
	iimavlib::rgb_t black (0,0,0);
	iimavlib::video_buffer_t data(sdl.get_size(),black);
	sdl.start();

	double angle = 0.0;
	iimavlib::rectangle_t position (data.size.width/2, data.size.height/2, 128, 128);
	iimavlib::video_buffer_t bg(data.size,black);
	mandelbrot(bg,50);

	while(sdl.blit(data)) {
		data = bg;

		position.x = static_cast<int>(400 - 64 + std::cos(angle) * 200);
		position.y = static_cast<int>(300 - 64 + std::sin(angle) * 150);

		const auto size = 64 + 64 * (1.0 + sin(angle))*(1.0 +cos(angle));
		position.width = position.height = static_cast<int>(size);

		draw_circle (data, position, iimavlib::rgb_t(255,0,0));


		std::vector<iimavlib::rectangle_t> points;
		for (int i = 0; i<6;++i) {
			points.push_back(iimavlib::rectangle_t(static_cast<int>(400 + sin(angle+i*6.28/6)*200), static_cast<int>(300 + cos(i*6.28/6)*200)));
		}

		draw_polygon(data, points, iimavlib::rgb_t(255,255,255));
		angle += 0.01;

#ifdef SYSTEM_LINUX
		usleep(1000);
#endif
	}
	sdl.stop();

}


