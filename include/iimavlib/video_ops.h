/*!
 * @file 		video_ops.h
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		25.2.2014
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
 */


#ifndef VIDEO_OPS_H_
#define VIDEO_OPS_H_
#include "video_types.h"


namespace iimavlib {

void blit(video_buffer_t& target, const video_buffer_t& src, rectangle_t position);

void draw_circle(video_buffer_t& data, rectangle_t rectangle, rgb_t color);


void draw_rectangle(video_buffer_t& data, rectangle_t rectangle, rgb_t color);

void draw_empty_rectangle(video_buffer_t& data, rectangle_t rectangle, int border, rgb_t color);

void draw_line(iimavlib::video_buffer_t& data, iimavlib::rectangle_t start, iimavlib::rectangle_t end, iimavlib::rgb_t color);

void draw_line_thick(iimavlib::video_buffer_t& data, iimavlib::rectangle_t start, iimavlib::rectangle_t end, int border, iimavlib::rgb_t color);


void draw_polyline(iimavlib::video_buffer_t& data, const std::vector<iimavlib::rectangle_t>& points, iimavlib::rgb_t color);

void draw_polygon(iimavlib::video_buffer_t& data, const std::vector<iimavlib::rectangle_t>& points, iimavlib::rgb_t color);

}
#endif /* VIDEO_OPS_H_ */
