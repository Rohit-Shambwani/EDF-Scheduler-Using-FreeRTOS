/*
 * Projecttask1.h
 *
 *  Created on: 28-Nov-2021
 *      Author: Rohit Shambwani
 */
#include <time.h>
#include<stdio.h>
#include<stdlib.h>
#include "FreeRTOS.h"

#define temppriority tskIDLE_PRIORITY+3
#define htpriority tskIDLE_PRIORITY+2
#define prespriority tskIDLE_PRIORITY+1
#define edfpriority tskIDLE_PRIORITY+4

#define tempperiod 2
#define presperiod 3
#define htperiod 5

#ifndef FREERTOS_SOURCE_INCLUDE_PROJECTTASK1_H_
#define FREERTOS_SOURCE_INCLUDE_PROJECTTASK1_H_

extern int gettemperature();
extern int getpressure();
extern int getheight();

#endif /* FREERTOS_SOURCE_INCLUDE_PROJECTTASK1_H_ */
