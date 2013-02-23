/**
 * @file 	sdl_window.cpp
 *
 * @date 	23.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimaudio/SDLDevice.h"
#include <unistd.h>
#include <algorithm>
int main()
{
	iimaudio::SDLDevice sdl(800,600,"HEY!");
	iimaudio::SDLDevice::data_type data(800*600);
	sdl.start();
	while(sdl.update(data)) {
		std::for_each(data.begin(),data.end(),[](iimaudio::RGB&rgb){rgb.r+=1;});
		usleep(1000);
	}
	sdl.stop();

}

