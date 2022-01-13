

/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE 1: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 * NOTE 2:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky version.  Console output
 * is used in place of the normal LED toggling.
 *
 * NOTE 3:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, are defined
 * in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, one software timer, and two tasks.  It then
 * starts the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  It uses vTaskDelayUntil() to create a periodic task that sends
 * the value 100 to the queue every 200 milliseconds (please read the notes
 * above regarding the accuracy of timing under Windows).
 *
 * The Queue Send Software Timer:
 * The timer is an auto-reload timer with a period of two seconds.  The timer's
 * callback function writes the value 200 to the queue.  The callback function
 * is implemented by prvQueueSendTimerCallback() within this file.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() waits for data to arrive on the queue.
 * When data is received, the task checks the value of the data, then outputs a
 * message to indicate if the data came from the queue send task or the queue
 * send software timer.
 *
 * Expected Behaviour:
 * - The queue send task writes to the queue every 200ms, so every 200ms the
 *   queue receive task will output a message indicating that data was received
 *   on the queue from the queue send task.
 * - The queue send software timer has a period of two seconds, and is reset
 *   each time a key is pressed.  So if two seconds expire without a key being
 *   pressed then the queue receive task will output a message indicating that
 *   data was received on the queue from the queue send software timer.
 *
 * NOTE:  Console input and output relies on Windows system calls, which can
 * interfere with the execution of the FreeRTOS Windows port.  This demo only
 * uses Windows system call occasionally.  Heavier use of Windows system calls
 * can crash the port.
 */

/* Standard includes. */
#include<stdio.h>
#include<FreeRTOS.h>
#include"task.h"
#include"Projecttask1.h"
#define HEAP_SIZE 3

typedef struct heap{
	int info[HEAP_SIZE + 1];
	int num_elements;
}heap;

int executed_task[3]={0,0,0};
int input_periods[3]={tempperiod,htperiod,presperiod};
int deadlines[3]={tempperiod,htperiod,presperiod};


TaskHandle_t task1Hand1= NULL;
TaskHandle_t task2Hand1= NULL;
TaskHandle_t task3Hand1= NULL;
TaskHandle_t task4Hand1= NULL;
//TaskHandle_t schedulertask= NULL;

int find_lcm(int a,int b){
	int temp=a;
	while(1){
	if(temp%b==0&&temp%a==0)
	break;
	temp++;
	}
	return temp;
}

int hyper_period(int p1,int p2,int p3){
	int l,k;
	if(p1>p2){l=find_lcm(p1,p2);}
	else{l=find_lcm(p2,p1);}
	if(l>p3){k=find_lcm(l,p3);}
	else{k=find_lcm(p3,l);}
	return k;
}

int bin_search(int arr[],int x,int down,int up){
  while (down<= up){
    int mid=down+(up-down)/2;
    if (arr[mid]==x)
      return mid;
    if (arr[mid]<x)
      down=mid+1;
    else
      up=mid-1;
  }
  return -1;
}

void heap_initialize(heap *heap){
	heap->num_elements=0;
	int len = sizeof(heap->info)/sizeof(int);
	for(int i=0;i<len;i++){
		heap->info[i]=0;
	}
}

int minimum_element(heap *heap){
	return heap->info[1];
}


int parent_of(int n){
	return n == 1 ? -1 : (int)(n/2);

}

int num_child(int n){
	return 2*n;
}

void heap_down(heap *heap, int p){
	int c;
	int i;
	int min_index;
	c = num_child(p);
	min_index = p;
	for(i = 0; i < 2; i++){
		if(c + i <= heap -> num_elements){
			if(heap -> info[min_index] > heap -> info[c+i]){
				min_index = c + i;
			}
		}
	}
	if(min_index != p){
		int temp_variable = heap -> info[p];
		heap -> info[p] = heap -> info[min_index];
		heap -> info[min_index] = temp_variable;
		heap_down(heap, min_index);
	}
}

void heap_up(heap *heap, int index){
	if(parent_of(index) == -1)
		return;
	if(heap -> info[parent_of(index)] > heap -> info[index]){
		int temp_variable = heap -> info[parent_of(index)];
		heap -> info[parent_of(index)] = heap -> info[index];
		heap -> info[index] = temp_variable;
		heap_up(heap, parent_of(index));
	}
}

int delete_minimum(heap *heap){
	int min = -1;
	if(heap->num_elements <= 0)
		printf("EMPTY HEAP!");
	else{
		min=heap->info[1];
		heap->info[1]=heap->info[heap->num_elements];
		heap->num_elements-=1;
		heap_down(heap,1);
	}
	return min;
}

void heap_insert(heap *heap, int x){
	if(heap -> num_elements >= HEAP_SIZE)
		printf("WARNING: HEAP OVERFLOW ERROR!\n");
	else{
		heap->num_elements+=1;
		heap->info[heap->num_elements]=x;
		heap_up(heap,heap->num_elements);
	}
}


void mytask4(void *p)
{
	//int hp=hyper_period(input_periods[0],input_periods[1],input_periods[2]);
	TickType_t xLastWakeTime;

	heap *my_heap = malloc(sizeof(heap));
	printf("hi");
	fflush(stdout);

	while(1){
		xLastWakeTime = xTaskGetTickCount();
		int to_schedule=0;
		int task_num=0;
		int tick_count=xTaskGetTickCount();
		int flags[3]={0,0,0};
		heap_initialize(my_heap);
		for(int i=0;i<3;i++){
			if(tick_count%input_periods[i]==0){
				flags[i]=1;
			}
			if(flags[i]==1){
			deadlines[i]=deadlines[i]+input_periods[i];
			heap_insert(my_heap,deadlines[i]);
			}
		}
		int k=1;
		for(int j=0;j<3;j++)
		{
			if(flags[j]==1)
			{
				to_schedule=minimum_element(my_heap);
				task_num=bin_search(deadlines,to_schedule,0,2);
				delete_minimum(my_heap);
				if(task_num==1){vTaskPrioritySet(task1Hand1,k++);}
				if(task_num==2){vTaskPrioritySet(task2Hand1,k++);}
				if(task_num==3){vTaskPrioritySet(task3Hand1,k++);}
			}
		}
		//vTaskDelay(1000/portTICK_PERIOD_MS);
		vTaskDelayUntil(&xLastWakeTime,1);
	}
}




void mytask1(void *p){
	int temp;
	TickType_t currtick;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1){
	temp=gettemperature();
	currtick=xTaskGetTickCount();
	//vTaskDelayUntil( &xLastWakeTime,tempperiod*1000/portTICK_PERIOD_MS);
	printf("Task 1\nMy Temperature reading is %d ,Current Tick Task 1 is %d\n",temp,currtick);
	fflush(stdout);
	vTaskDelayUntil( &xLastWakeTime,tempperiod*1000/portTICK_PERIOD_MS);
	}
	//vTaskDelay(1000/portTICK_PERIOD_MS);
}
void mytask2(void *p){
	TickType_t currtick;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	int ht;
	while(1){
	//printf("Task 2\n");
	ht=getheight();
	//fflush(stdout);
	currtick=xTaskGetTickCount();
	printf("Task 2 \nMy Height reading is %d, Current Tick Task2 Time is%d\n\r",ht,currtick);
	fflush(stdout);
	vTaskDelayUntil( &xLastWakeTime,htperiod*1000/portTICK_PERIOD_MS);
	}
	//vTaskDelay(2000/portTICK_PERIOD_MS);
}

void mytask3(void *p){
	TickType_t currtick;
	TickType_t xLastWakeTime1;
	xLastWakeTime1= xTaskGetTickCount();
	int pres;
	while(1){
	//printf("Task 3\n");
	//fflush(stdout);
	pres=getpressure();
	//printf("My Pressure reading is %d\n\r",pres);
	//fflush(stdout);
	currtick=xTaskGetTickCount();
	printf("Task 3 \nMy Pressure reading is %d, Current Tick Task3 Time is%d\n\r",pres,currtick);
	//printf("Current Tick T is %d\n\r",xLastWakeTime1);
	fflush(stdout);
	vTaskDelayUntil( &xLastWakeTime1, presperiod*1000/portTICK_PERIOD_MS );}
	//vTaskDelay(3000/portTICK_PERIOD_MS);
}

void vscheduleEDF(){
	xTaskCreate(mytask4,"T1",200,(void*)0,edfpriority,&task4Hand1);

}
void createtasks(){
	xTaskCreate(mytask1,"T1",200,(void*)0,temppriority,&task1Hand1);
	xTaskCreate(mytask2,"T2",200,(void*)0,htpriority,&task2Hand1);
	xTaskCreate(mytask3,"T3",200,(void*)0,prespriority,&task3Hand1);
}

void main_blinky( void )
{
	createtasks();
	vscheduleEDF();
	vTaskStartScheduler();
	while(1);
}
/*-----------------------------------------------------------*/


