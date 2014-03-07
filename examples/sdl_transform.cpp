/**
 * @file 	sdl_window.cpp
 *
 * @date 	23.2.2013
 * @author 	Zdenek Travnicek <travnicek@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

//TRANSFORMACE MOZNO NASTUDOVAT V KNIZE J. ZARA a kol.: MODERNI POCITACOVA GRAFIKA 

#include "iimavlib/SDLDevice.h"
#include "iimavlib/Utils.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <algorithm>
#include <iostream>
///*
#include <cmath>

const int xMax=500;
const int yMax=500;
const double PI=3.14159265;

//prohozeni obsahu dvou vektoru - nepouzito
template<typename T>void swapt(T &p1,T &p2) {
	T tmp=std::move(p1);
	p1=std::move(p2);
	p2=std::move(tmp);
}

//zaokrouhleni
int round_impl(double x) {
#ifdef SYSTEM_LINUX
	return std::round(x);
#else
	return (int) (x+0.5);
#endif
}

//posunuti o dx,dy
template<typename T> void shiftbmp(T&d, T&s, int dx, int dy) {
	int xn,yn,x,y;
	for(y=0;y<yMax;y++) {
		for(x=0;x<xMax;x++) {
			xn=(x-dx+xMax)%xMax;
			yn=(y-dy+yMax)%yMax;
			d(xn,yn)=s(x, y);
		}
	}
}

//rotace okolo leveho horniho rohu
template<typename T> void rot0(T&d, T&s, double deg) {
	const double omega=2*PI*deg/360.0;
	const double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;
	
	for(y=0;y<yMax;y++) {
		for(x=0;x<xMax;x++) {
			xn=round_impl(x*cosT+y*sinT);
			yn=round_impl(y*cosT-x*sinT);
			if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
				d(x,y)=s(xn, yn);
		}
	}
}

//rotace okolo stredu
template<typename T> void rot(T&d, T&s, double deg) {
	const double omega=2*PI*deg/360.0;
	const double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;
	const int cx=xMax/2;
	const int cy=yMax/2;

	for(y=0;y<yMax;y++) {
		for(x=0;x<xMax;x++) {
			xn=round_impl(  (x-cx)*cosT+(y-cy)*sinT+cx   );
			yn=round_impl(  (y-cy)*cosT-(x-cx)*sinT+cy     );
			if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
				d(x, y)=s(xn, yn);
		}
	}
}

//rotace okolo bodu cx,cy
template<typename T> void rotc(T&d, T&s, double deg, int cx=0, int cy=0) {
	const double omega=2*PI*deg/360.0;
	const double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;

	for(y=0;y<yMax;y++) {
		for(x=0;x<xMax;x++) {
			xn=round_impl(  (x-cx)*cosT+(y-cy)*sinT+cx   );
			yn=round_impl(  (y-cy)*cosT-(x-cx)*sinT+cy     );
			if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
				d(x,y)=s(xn, yn);
		}
	}
}

//rotace sub-mapy x1,y1,x2,y2 okolo bodu cx,cy
template<typename T> void rotcsub(T&d, T&s, double deg, int cx=0, int cy=0,int x1=0, int y1=0, int x2=xMax, int y2=yMax) {

	const double omega=2*PI*deg/360.0;
	const double cosT=cos(omega), sinT=sin(omega);
	int xn,yn,x,y;

	if(x1>=0 && x1<=xMax &&
		y1>=0 && y1<=yMax &&
		x2>=0 && x2<=xMax &&
		y2>=0 && y2<=yMax) {

		for(y=y1;y<y2;y++) {
			for(x=x1;x<x2;x++) {
				xn=round_impl(  (x-cx)*cosT+(y-cy)*sinT+cx   ); 
				yn=round_impl(  (y-cy)*cosT-(x-cx)*sinT+cy     );
				if(xn>=0 && xn<xMax && yn>=0 && yn<yMax) 
					d(x,y)=s(xn, yn);
			}
		}
	}
}


int main()
{
	iimavlib::SDLDevice sdl(xMax,yMax,"TRANSFORM");
	iimavlib::video_buffer_t data(xMax, yMax),rx(xMax, yMax);
	
	int i=0,j=0;
	bool swtch=false;
	bool swtch2=false;
	
	int rectx1=xMax/4,recty1=yMax/4,rectx2=xMax-xMax/4,recty2=yMax-yMax/4;
	int cx=xMax/2, cy=yMax/2;

	//inicializace bitmapy
	std::for_each(data.begin(),data.end(),[&](iimavlib::rgb_t&rgb){
			
		//inicializace - cerne pozadi
		rgb.r=rgb.g=rgb.b=0;
		//zapnout/vypnout inkoust kazdych 10 sloupcu
		if(i%10==0) 
			swtch=!swtch;
		//pokud je zapnuty inkoust a jsem v kreslici sub-mape kreslim bilou barvou
		if((swtch || swtch2)  && i>=rectx1 && i < rectx2 && j>=recty1 && j<recty2)
			rgb.r=rgb.g=rgb.b=255;
		
		//poskocit na pixel napravo, kdyz uz jsem na konci skoci to automaticky na zacatek
		i=(i+1)%xMax;
		
		//odradkovani pred zacatkem radku
		if(i==0) {
			j=(j+1)%yMax;
			//zapnout/vypnout inkoust kazdych 10 radek
			if(j%10==0) swtch2=!swtch2;
		}
		});

	const double angle=6; //degrees

	//nastartovani noveho vlakna
	sdl.start();
	//hlavni vykreslovaci smycka
	while(sdl.blit(data)) {
//		int i=0,j=0;
		//prohozeni bitmap v pameti - do jedne se kreslilo a ted se bude zobrazovat 
		//a do te druhe se ted bude kreslit a v dalsim cyklu se zobrazi
		std::swap(data,rx);

		//rotace bitmapy
		rotcsub<iimavlib::video_buffer_t>(data,rx,angle,cx,cy);
		//posunuti bitmapy
		//shiftbmp<iimavlib::SDLDevice::data_type>(data,rx,1,1);
		
#ifdef SYSTEM_LINUX
		usleep(1000);
#else
		Sleep(10);
#endif
	}
	sdl.stop();

}

