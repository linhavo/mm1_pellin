/**
 * @file 	cellular_automata.cpp
 *
 * @date 	20.3.2013
 * @date 	5.3.2014
 * @author 	Roman Berka <berka@iim.cz>
 * 			Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#include "iimavlib/Utils.h"
#include "iimavlib/video_ops.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <algorithm>
#include <iostream>

#include <cmath>
#include <vector>
#include <random>

const iimavlib::rectangle_t window_size(0, 0, 800, 800);
const iimavlib::rectangle_t cell_resolution (0, 0, 100, 100);
const iimavlib::rgb_t black (0, 0, 0);

namespace automata {

using namespace iimavlib;
// definition of available states for cells
EXPORT enum ca_state {
    healthful = 0,
    ill = 2,
    dead = 3,
    empty = 4
};

// the table assigning colors to given states of cells declared above
const std::map<ca_state, rgb_t> color_tab = InitMap<ca_state, rgb_t>
	(healthful, rgb_t( 0, 255, 0 ))
    (ill, rgb_t( 255, 0, 0 ))
    (dead, rgb_t( 0, 0, 0 ))
    (empty, rgb_t( 255, 255, 255 ));

// declaration of one cell (state, age)
struct ca_cell {
    ca_state state;
    int age;
    int x;
    int y;
};

typedef std::vector<ca_cell> cell_vector_t;

class Automata: public iimavlib::SDLDevice
{
public:
	Automata(rectangle_t window_size, rectangle_t cell_resolution);
	~Automata() /*noexcept */{}
	void run();
private:
	video_buffer_t data_;
	iimavlib::rectangle_t cell_resolution_;
	iimavlib::rectangle_t cell_size_;
	cell_vector_t current_cells_;
	cell_vector_t next_cells_;
	std::random_device rand;
	std::uniform_int_distribution<int> idistrib;

	void eval();
	void draw();
	ca_cell new_state(const cell_vector_t& ca, int i, int j);
	ca_cell scan_surrounding(const cell_vector_t& cells, int i, int j) const;
};


}

int main()
{
    automata::Automata automat(window_size, cell_resolution);
    automat.run();
}


namespace automata {

Automata::Automata(rectangle_t window_size, rectangle_t cell_resolution)
:SDLDevice(window_size.width, window_size.height),data_(window_size, black),
cell_resolution_(cell_resolution),cell_size_(0,0,window_size.width/cell_resolution.width, window_size.height/cell_resolution.height),
idistrib(-40,20)
{
	current_cells_.resize(cell_resolution_.width * cell_resolution_.height);
	next_cells_.resize(cell_resolution_.width * cell_resolution_.height);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

	for (int y = 0; y < cell_resolution_.height; ++y) {
		for (int x = 0; x < cell_resolution_.width; ++x) {
			auto &cc = current_cells_[y * cell_resolution_.width + x];
			auto &nc = next_cells_[y * cell_resolution_.width + x];
			cc.x = x;
			cc.y = y;
			nc.x = x;
			nc.y = y;
			double u = distribution(rand);
			if (u > .99)
			  cc.state = ill;
			else if (u < .4)
			  cc.state = healthful;
			else
			  cc.state = empty;
		}
	}

}

void Automata::run ()
{
	start();
	while (blit(data_)) {
		eval();
		std::swap(current_cells_,next_cells_);
	    draw();
	}
	stop();
}


// computing new states for next generation of cellular automata (stored in ca1)
// based on current
// state of each cell and its neighbour cells (stored in ca2)
void Automata::eval()
{
	for (auto& cell: next_cells_) {
            cell = new_state(current_cells_, cell.x, cell.y);
	}
}



// draw whole new bitmap (cabm) based on data from cellular automata (ca)
void Automata::draw()
{
	for (const auto& cell: current_cells_) {
		rectangle_t position (cell_size_.width*cell.x, cell_size_.height*cell.y,cell_size_.width, cell_size_.height);
		draw_rectangle(data_, position, color_tab.at(cell.state));
    }
}



ca_cell Automata::scan_surrounding(const cell_vector_t& cells, int x, int y) const
{
	ca_cell state = { empty, 0 ,0,0};
	std::map<ca_state, int> histogram;
	const int min_y = std::max(y - 1, 0);
	const int min_x = std::max(x - 1, 0);
	const int max_y = std::min(y + 2, cell_resolution_.width);
	const int max_x = std::min(x + 2, cell_resolution_.height);
	for (int line = min_y; line < max_y; ++line) {
		for (int row = min_x; row < max_x; ++row) {
			const ca_cell cell = cells[cell_resolution_.width * line + row];
			state.age += cell.age;
			histogram[cell.state]++;
			if (cell.state == ill) histogram[cell.state]+=1;
			//state_sum += cell.state;
		}
	}
	const int cell_count = (max_y - min_y)*(max_x - min_x);
	state.age /= cell_count;
	auto me = std::max_element(histogram.begin(), histogram.end(), [](const std::pair<ca_state,int>& a, const std::pair<ca_state,int>& b)
			{return a.second < b.second;});
	state.state = me->first;
	return state;
}





ca_cell Automata::new_state(const cell_vector_t& ca, int x, int y)
{
    ca_cell state = scan_surrounding(ca, x, y);
    ca_cell target = ca[x * cell_resolution.width + y];


    // rules - the central place for the rule definitions
    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

    if (target.state == healthful && state.state == ill) {
		target.state = ill;
		target.age = 0;
	}

    target.age++;

    switch (target.state) {
		case healthful:
			if (target.age > 50) {
				target.state = ill;
				target.age = idistrib(rand);
			}
			break;
		case ill:

			if (target.age > 20) {
				target.state = dead;
				target.age = idistrib(rand);
			}
			break;
		case dead:
			if (target.age > 80) {
				target.state = empty;
				target.age = idistrib(rand);
			} break;
		case empty:
			if (target.age > 200 || (target.age > 120 && state.state == healthful)) {
				target.state = healthful;
				target.age = idistrib(rand);
			} break;
	}
    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    return target;
}


}
