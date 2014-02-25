/**
 * @file 	sdl_cellular_automata.cpp
 *
 * @date 	20.3.2013
 * @author 	Roman Berka <berka@iim.cz>
 * @copyright GNU Public License 3.0
 *
 */

#include "iimavlib/SDLDevice.h"
#include "iimavlib/Utils.h"
#ifdef SYSTEM_LINUX
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <algorithm>
#include <iostream>

#include <cmath>
#include<vector>

const int xMax=500;
const int yMax=500;

const int cellx=100;
const int celly=100;

//cell dimensions in pixels
const int hx=xMax/cellx;
const int hy=yMax/celly;

//definition of available states for cells
typedef enum{healthful=0,live=1,ill=2,dead=3,empty=4} ca_state; 
//declaration of one cell (state, age)
typedef struct{ca_state st; int age;} ca_cell;//nezapomenout inicializovat instanci
//the table assigning colors to given states of cells declared above (5 states - 5 colors)
const iimavlib::rgb_t color_tab[]={{0,255,0},{0,0,255},{255,0,0},{0,0,0},{255,255,255}};

//round double argument
int round_impl(double x) {
#ifdef SYSTEM_LINUX
	return std::round(x);
#else
	return (int) (x+0.5);
#endif
}

template<typename T>ca_cell new_state(T&ca, int i, int j) {
	ca_cell state={empty,0};
	ca_cell target=ca[i*cellx+j];
	int illc=0,st=0,cr,cc,k,l;

	//compute average age and fitnes of surounding cells
	for(cr=0,k=(i-1+celly)%celly;cr<3;cr++,k=(k+1+celly)%celly)
		for(cc=0,l=(j-1+cellx)%cellx;cc<3;cc++,l=(l+1+cellx)%cellx)
			if(!(k==i && l==j)) {
				state.age+=ca[k*cellx+l].age;
				st+=ca[k*cellx+l].st;
				if(ca[k*cellx+l].st!=ill) illc++;
			}
	state.age = round_impl(state.age/8.0f);
	state.st = (ca_state)round_impl(st/8.0f);

	//rules - the central place for the rule definitions
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	int dstate=abs(target.st-state.st);
	if(target.age>1000 && target.st==healthful) {target.st=ill; /*target.age=-1;*/}
	else if(illc>=6 && target.st==healthful) target.st=live;
			else if(dstate>=1) {
				target.st=state.st;
				if(target.st==dead || target.st==empty) target.age=-1;
			} else if(target.st==live && illc>=6) target.st=ill;
			else if(target.st==ill && target.age>600 && illc>=6) {target.st=dead;target.age=-1;}

	switch(target.st) {
	case healthful:
		target.age++;
		break;
	case live:target.age+=2;break;
	case ill:target.age+=3;break;
	case dead:if(target.age>40) {target.st=empty;target.age=0;} else target.age++;break;
	default: if(target.age>60) {target.st=healthful;target.age=0;} else target.age++;
	}
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	return target;
}


//computing new states for next generation of cellular automata (stored in ca1) based on current 
//state of each cell and its neighbour cells (stored in ca2)
template<typename T>void eval(T&ca1,T&ca2) {
	for(int i=0;i<celly;i++)
		for(int j=0;j<cellx;j++)
			ca1[i*cellx+j]=new_state(ca2,i,j);
}

//based on data from automata (stored in ca) fills cell [i,j] in bitmap (stored in cabm)
template<typename T1, typename T2>void fill_cell(T1&cabm, T2&ca, int i, int j) {
	iimavlib::rgb_t rgb=color_tab[ca[i*cellx+j].st];
	int sx=j*hx,sy=i*hy,xstop=sx+hx,ystop=sy+hy;

	for(int k=sy;k<ystop;k++)
		for(int l=sx;l<xstop;l++)
			cabm(k,l)=rgb;
}

//draw whole new bitmap (cabm) based on data from cellular automata (ca) 
template<typename T1, typename T2>void draw(T1&cabm, T2&ca) {
	for(int i=0;i<celly;i++)
		for(int j=0;j<cellx;j++) 
			fill_cell(cabm,ca,i,j);
}

int main()
{
	std::vector<ca_cell> ca1(cellx*celly), ca2(cellx*celly);
	iimavlib::SDLDevice sdl(xMax,yMax,"CELLULAR");
	iimavlib::video_buffer_t data(xMax, yMax);
	
//	int i=0,j=0;
	
	//the bitmap initialization
	std::for_each(ca2.begin(),ca2.end(),[](ca_cell&cl){
		cl.age=0;
		double u = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1.0);
		if(u>.99) cl.st = ill;
		else if(u<.4) cl.st=healthful;
		else cl.st=empty;
	});
	draw(data,ca2);
	
	//start new thread
	sdl.start();
	//the main drawing loop
//	i=0;
	while(sdl.blit(data)) {
		eval(ca1,ca2);
		draw(data,ca1);
		std::swap(ca1,ca2);
#ifdef SYSTEM_LINUX
		usleep(1000);
#else
		//Sleep(1000);
#endif
	}
	sdl.stop();

}

