/*
 * projecttask1.c
 *
 *  Created on: 28-Nov-2021
 *      Author: Rohit Shambwani
 */
#include "Projecttask1.h"
int gettemperature(){
	int resulttemp;
	int ub;
	int lb;
	ub=100;
	lb=20;
	srand(time(0));
	resulttemp=(rand()%(ub-lb+1))+lb;
	return resulttemp;

}
int getpressure(){
	int resultpres;
	int ub;
	int lb;
	ub=10;
	lb=1;
	srand(time(0));
	resultpres=(rand()%(ub-lb+1))+lb;
	return resultpres;

}
int getheight(){
	int resultht;
	int ub;
	int lb;
	ub=1000;
	lb=0;
	srand(time(0));
	resultht=(rand()%(ub-lb+1))+lb;
	return resultht;

}


