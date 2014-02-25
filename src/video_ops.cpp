/*!
 * @file 		video_ops.cpp
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		25.2.2014
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
 */

#include "iimavlib/video_ops.h"
#include <cmath>

namespace iimavlib {


void blit(video_buffer_t& target, const video_buffer_t& src, rectangle_t position)
{
	if (src.size.x != 0 || src.size.y != 0) throw std::runtime_error("Video buffer has to be placed at 0,0!");
	if (position.width < 0) {
		position.width = src.size.width;
	}
	if (position.height < 0) {
		position.height = src.size.height;
	}

	// Area of input buffer, that will be copied
//	position = intersection(src.size, position);

	// Area of internal buffer that will be overwritten
	rectangle_t target_res = intersection(position, target.size);


	for (int line = 0; line < target_res.height; ++line) {
		auto input_iter = src.begin() + line * src.size.width;
		auto input_iter_end = input_iter + target_res.width;
		auto output_iter = target.begin() + (line + position.y)* target.size.width + position.y;
		std::copy(input_iter, input_iter_end, output_iter);
	}
}

void draw_circle(video_buffer_t& data, rectangle_t rectangle, rgb_t color)
{
	rectangle_t target = intersection(data.size, rectangle);
	const int min_line = target.y;
	const int max_line = target.y + target.height;
	const int middle_line = rectangle.y + rectangle.height / 2;


	auto base_ptr = data.begin()+min_line * data.size.width;
	for (int l = min_line; l < max_line; ++l) {
		const int 	line 	= l - middle_line;

		const int 	x_diff 	= rectangle.width * std::sqrt(1-std::pow(2.0*line/rectangle.height,2.0)) / 2;
		int 		x0 		= std::max(rectangle.x + rectangle.width / 2 - x_diff, 0);
		const int 	x1 		= std::min(rectangle.x + rectangle.width / 2 + x_diff, data.size.width);
		auto 		ptr 	= base_ptr + x0;
		base_ptr += data.size.width;
		if (x0>=x1) continue;
		while (x0++ < x1) *ptr++=color;
	}


}


void draw_rectangle(video_buffer_t& data, rectangle_t rectangle, rgb_t color)
{
	rectangle = intersection(data.size, rectangle);
	for (int line = rectangle.y; line < rectangle.y + rectangle.height; ++line) {
		auto iter = data.begin() + line * data.size.width + rectangle.x;
		std::fill(iter, iter+ rectangle.width, color);
	}
}

void draw_empty_rectangle(video_buffer_t& data, rectangle_t rectangle, int border, rgb_t color)
{
	rectangle = intersection(data.size, rectangle);

	// Draw top part
	draw_rectangle(data, {rectangle.x, rectangle.y, rectangle.width, border}, color);
	// Draw bottom part
	draw_rectangle(data, {rectangle.x, rectangle.y+ rectangle.height - border,
					rectangle.width, border}, color);
	// Draw left part
	draw_rectangle(data, {rectangle.x, rectangle.y + border,
					border, rectangle.height-2*border}, color);
	// Draw right part
	draw_rectangle(data, {rectangle.x + rectangle.width - border, rectangle.y + border,
					border, rectangle.height- 2* border}, color);
}

void draw_line_steep(iimavlib::video_buffer_t& data, iimavlib::rectangle_t start, iimavlib::rectangle_t end, iimavlib::rgb_t color)
{
	if (start.y > end.y) std::swap(start,end);
	const int x_length = end.x - start.x;
	const int y_length = end.y - start.y;
	for (int row = start.y; row < end.y; ++row) {
		if (row < 0 || row >= data.size.height) continue;
		const int x = start.x + x_length * (row - start.y) / y_length;
		if (x < 0 || x >= data.size.height) continue;
		data(x,row) = color;
	}
}

void draw_line(iimavlib::video_buffer_t& data, iimavlib::rectangle_t start, iimavlib::rectangle_t end, iimavlib::rgb_t color)
{
	if (std::abs(end.y-start.y) > std::abs(end.x-start.x)) return draw_line_steep(data,start,end,color);
	if (start.x > end.x) std::swap(start,end);
	const int x_length = end.x - start.x;
	const int y_length = end.y - start.y;
	for (int col = start.x; col < end.x; ++col) {
		if (col < 0 || col >= data.size.width) continue;
		const int y = start.y + y_length * (col - start.x) / x_length;
		if (y < 0 || y >= data.size.height) continue;
		data(col,y) = color;
	}
}

void draw_line_thick(iimavlib::video_buffer_t& data, iimavlib::rectangle_t start, iimavlib::rectangle_t end, int border, iimavlib::rgb_t color)
{
	const int half_border = border/2;
	for (int i = 0; i < border; ++i) {

		draw_line(data, {start.x, start.y +i - half_border, 0, 0},
						{end.x, end.y +i - half_border, 0, 0},color);
	}
}


void draw_polyline(iimavlib::video_buffer_t& data, const std::vector<iimavlib::rectangle_t>& points, iimavlib::rgb_t color)
{
	std::accumulate(points.begin()+1, points.end(), points[0],
		[color, &data](const iimavlib::rectangle_t& rect1, const iimavlib::rectangle_t& rect2)
		{draw_line(data,rect1,rect2,color);return rect2;});
}

void draw_polygon(iimavlib::video_buffer_t& data, const std::vector<iimavlib::rectangle_t>& points, iimavlib::rgb_t color)
{
	draw_polyline(data,points,color);
	draw_line(data,points.front(), points.back(), color);
}

}



