/**
 * @file 	sdl_window.cpp
 *
 * @date 	23.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#include "iimavlib/Utils.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#endif
#include <algorithm>
#include <iostream>
///*

const int xMax=800;
const int yMax=80;
const int cx=xMax/2;
const int cy=yMax/2;

int round(double x) {
	return (int) (x+0.5);
}

template<typename T> void move(T&d, T&s, int dx, int dy) {
	int xn,yn,x,y;
	for(y=0;y<yMax;y++) {
		for(x=0;x<xMax;x++) {
			xn=(x-dx+xMax)%xMax;
			yn=(y-dy+yMax)%yMax;
			d[y*xMax+x]=s[yn*xMax+xn];
		}
	}
}

template<typename T> void rot0(T&d, T&s, double deg) {
	const double PI=3.1415926;
	double omega=2*PI*deg/360.0;
	double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;
	
	for(y=0;y<yMax;y++) {
		x=0;
		for(x=0;x<xMax;x++) {
			xn=round(x*cosT+y*sinT);
			yn=round(y*cosT-x*sinT);
			
			if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
				d[y*xMax+x]=s[yn*xMax+xn];
			else {
				d[y*xMax+x].r=0;d[y*xMax+x].g=0;d[y*xMax+x].b=0;
			}
		}
	}
	d[0].r=100;	d[0].g=0;	d[0].b=0;
	s[0].r=0;	s[0].g=0;	s[0].b=100;
}

template<typename T> void rot(T&d, T&s, double deg) {
	const double PI=3.1415926;
	double omega=2*PI*deg/360.0;
	double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;
	for(y=0;y<yMax;y++) {
		xn=round((0-cx)*cosT+(y-cy)*sinT+cx);
		yn=round((y-cy)*cosT-(0-cx)*sinT+cy);
		for(x=0;x<xMax;x++) {
			if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
				d[y*xMax+x]=s[yn*xMax+xn];
			xn+=round(cosT);
			yn-=round(sinT);
		}
	}
}
//*/

template<typename T>void swapt(T &p1,T &p2) {
	T tmp=p1;
	p1=p2;
	p2=tmp;
}

int main()
{
	iimavlib::SDLDevice sdl(xMax,yMax,"ROT!");
	iimavlib::SDLDevice::data_type data(xMax*yMax),rx(xMax*yMax);
	iimavlib::SDLDevice::data_type *d=&data,*s=&rx;
	
	int i=0,j=0;
	bool swtch=false;
	//bool swtch2=false;
	//data[i].b=0;
	//rx[1]=data[1];
	//const double angle=3; //degrees

	/*for(i=0;i<xMax;i++) {
		if(j%10==0) swtch2=!swtch2;
		for(j=0;j<yMax;j++) {
			if(i%10==0) swtch=!swtch;
			
			if(swtch || swtch2) data[j*xMax+i].r=data[j*xMax+i].g=data[j*xMax+i].b=0;
			else data[j*xMax+i].r=data[j*xMax+i].g=data[j*xMax+i].b=255;

		}
		
	}*/
	
	std::for_each(data.begin(),data.end(),[&](iimavlib::RGB&rgb){
			
		rgb.r=rgb.g=(i/256)%256;
		rgb.b=i%256;
		//if(i%800==0) 
		//	swtch=!swtch;
		
		//if(swtch)
		//	rgb.r=rgb.g=rgb.b=255;
		
		i++;//=(i+1)%xMax;
		/*if(i==0) {
			j=(j+1)%yMax;
			if(j%10==0) swtch2=!swtch2;
		}*/
		});
	iimavlib::logger[iimavlib::log_level::info] << "i: " << i;
	sdl.start();
	while(sdl.update(data)) {
		//int i=0,j=0;
		//for (int n=0;n<1000000;n++);
		/*std::for_each(data.begin(),data.end(),[&](iimavlib::RGB&rgb){
			
			rgb.r=(rgb.r+1)%255;
			rgb.g=(rgb.g+1)%255;
			rgb.b=(rgb.b+1)%255;

			i=(i+1)%xMax;
			if(i==0) j=(j+1)%yMax;
		});*/
		//swapt<iimavlib::SDLDevice::data_type*>(d,s);
		//rot0<iimavlib::SDLDevice::data_type>(*d,*s,angle);
		//move<iimavlib::SDLDevice::data_type>(*d,*s,1,1);
		//std::cout << "iterace\n";

#ifdef SYSTEM_LINUX
		usleep(1000);
#endif
	}
	sdl.stop();

}

