#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include <wiringPi.h>
#include "control_task.h"
#include "utilities.h"
#include "common_helper.h"
#include "motordriver.h"
#include "posixTimer.h"

uint8_t current_state = STOPPED;
uint8_t ultrasonic_stop_flag = 0;
uint8_t ultrasonic_start_flag = 0;
uint8_t camera_start_flag = 0;
uint8_t camera_stop_flag = 0;
sem_t control_task_semaphore;
uint8_t stop_sign_detected = 0;
#define STOP_SIGN_TIMEOUT_msec 7000

timer_t timer_id2;

void camera_stop_detect_again(union sigval sig)
{
	stop_sign_detected = 0;
}

/*****************camera_stop_clear()********************/
//If stop sign detected the car will wait for seven seconds
//The control task is then signalled to start the motor
/********************************************************/
void camera_stop_clear(union sigval sig)
{
  if(current_state = CAMERA_STOPPED)
  {
    struct timespec timeNow;
    camera_start_flag = 1;
    clock_gettime(CLOCK_REALTIME, &timeNow);
    LOG_STDOUT(INFO "STOP SIGN timeout\n");  
    //LOG_STDOUT(INFO "STOP SIGN timeout. Setting event CAMERA START FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);  
    uint8_t ret = sem_post(&control_task_semaphore);
    if(ret)
    {
    	LOG_STDOUT(ERROR "SEMAPHORE POST FAILED\n");
    	//exit(ERROR);
    	//return ERROR;
    }
    if(start_timer(timer_id2 , 5000, 1) == -1)
    {
		  LOG_STDOUT("[ERROR] Start Timer\n");
		  //return ERR;
    }
	//stop_sign_detected = 0;
  }
  
}


/*********************control_task()*********************/
//Task running at highest priority
//Will be used for motor control based on the flags set by
//sonar sensor task and camera task
/********************************************************/
void* control_task(void *param) 
{	
  	timer_t timer_id;
   	if(register_timer(&timer_id, camera_stop_clear, &timer_id) == -1)
	{
   		LOG_STDOUT("[ERROR] Register Timer\n");
		return ERR;
	}
   	if(register_timer(&timer_id2, camera_stop_detect_again, &timer_id2) == -1)
	{
    		LOG_STDOUT("[ERROR] Register Timer2\n");
		return ERR;
	}
	Motor_Setup();		//Initialize dc motors
	Motor_Stop();		//Setting initial state of motor as STOP
  	uint8_t event = -1;
	struct timespec timeNow;
	int ret = sem_init(&control_task_semaphore, 0, 0);		//Initialising semaphore for motor control
	if(ret)
	{
		LOG_STDOUT(ERROR "SEMAPHORE CREATE FAILED\n");
		exit(1);
		//return ERROR;
	}

    	while (1) 
	{
        	sem_wait(&control_task_semaphore);			//Waiting on semaphore from sonar sensor task and camera task 
		//LOG_STDOUT(INFO "Semaphore obtained for control task\n");
		
        if (ultrasonic_stop_flag) 					
	{
            	ultrasonic_stop_flag=0;					//Clearing ultrasonic_sensor flag
            	Motor_Stop();						//Stopping motor if obstacle detected
		current_state = STOPPED;
		clock_gettime(CLOCK_REALTIME, &timeNow);
            	LOG_STDOUT(INFO "STOPPING... Obstacle detected.\n");
            	//LOG_STDOUT(INFO "STOPPING... Obstacle detected. Clearing event ULTRASONIC STOP FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
        }
        else if (camera_stop_flag) 
	{
		//This will start as timer, on timeout, it will post a semaphore to the control task and set the event to camera_start_flag
		stop_sign_detected = 1;
		camera_stop_flag = 0;					//Clearing camera stop flag
		Motor_Stop();						//Stopping motor if STOP sign detected
		current_state = CAMERA_STOPPED;	
    		camera_stop_flag = 0;
		clock_gettime(CLOCK_REALTIME, &timeNow);
    		LOG_STDOUT(INFO "STOP SIGN. Stopping for 7 seconds....\n");
    		//LOG_STDOUT(INFO "STOP SIGN. Stopping for 7 seconds.... Clearing event CAMERA STOPPED  FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
		//sleep(3);
    	
		if(start_timer(timer_id , STOP_SIGN_TIMEOUT_msec, 1) == -1)
		{
			LOG_STDOUT("[ERROR] Start Timer\n");
			return ERR;
		}
		//camera_start_flag = 1;	
		
        }
        else if (ultrasonic_start_flag || camera_start_flag)
	{
		event = 0;
		if(ultrasonic_start_flag) {  event = 0; ultrasonic_start_flag=0;}
		else if(camera_start_flag) { event = 1; camera_start_flag =0; }
            	Motor_Start();						//Starting the motor if no obstacle or STOP sign is detected
		current_state = RUNNING;
		clock_gettime(CLOCK_REALTIME, &timeNow);
            	if(event == 0) LOG_STDOUT(INFO "RUNNING... Obstacle removed.\n");
            	//if(event == 0) LOG_STDOUT(INFO "RUNNING... Obstacle removed. Clearing event ULTRASONIC START FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
            	else if(event == 1) LOG_STDOUT(INFO "RUNNING... STOP SIGN TIMEOUT.\n");
            	//else if(event == 1) LOG_STDOUT(INFO "RUNNING... STOP SIGN TIMEOUT. Clearing event CAMERA START FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
	}
    }
    
}
