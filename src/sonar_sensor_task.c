#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <semaphore.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "sonar_sensor_task.h"
#include "common_helper.h"
#include "utilities.h"

#ifdef TRUE
#undef TRUE
#endif

#define TRUE 1

#define TRIG 5
#define ECHO 6

#define DISTANCE_THRESHOLD_CM 	20
  

extern uint8_t current_state;
extern sem_t control_task_semaphore;
extern uint8_t ultrasonic_stop_flag;
extern uint8_t ultrasonic_start_flag;

/*************************setup()*************************/
//The ultrasoni sensor is initialized using this function
/*********************************************************/
void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);

        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}

/*********************getDistance()***********************/
//This function will give the distance of the obstacle from the sensor.
//If no obstacle is detected within 2 milliseconds the function will be timed out
/*********************************************************/
int getDistance() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(TRIG, LOW);

        //Wait for echo start
        while(digitalRead(ECHO) == LOW);

        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH && ((micros()-startTime) < 2000));
        long travelTime = micros() - startTime;

        //Get distance in cm
        int distance = travelTime / 58;

        return distance;
}

/**************** sonar sensor task()*********************/
//This task will be running at second highest priority.
//This task is spawned every 300 ms to check for any obstacle
//If any obstacle is detected, the flags are set accordingly and the control task is signalled
/*********************************************************/
void* sonar_sensor_task(void *param) 
{
	int ret = 0;
	struct timespec timeNow, timeOld;
	clock_gettime(CLOCK_REALTIME, &timeOld);
	LOG_STDOUT(INFO "Setup of Sonar sensor begin @ %d sec %d nsec\n", timeOld.tv_sec, timeOld.tv_nsec);
    	setup();
	clock_gettime(CLOCK_REALTIME, &timeNow);
	LOG_STDOUT(INFO "Setup of Sonar sensor end @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
	
	int distance_cm = 0;
 
 	clock_gettime(CLOCK_REALTIME, &timeOld);
	LOG_STDOUT(INFO "getDistance begin @ %d sec %d nsec\n", timeOld.tv_sec, timeOld.tv_nsec);
	distance_cm = getDistance();
	clock_gettime(CLOCK_REALTIME, &timeNow);
	LOG_STDOUT(INFO "getDistance end @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
	
	while(1)
	{
		//printf("Distance: %dcm\n", getDistance());
		//clock_gettime(CLOCK_REALTIME, &timeOld);
		//LOG_STDOUT(INFO "getDistance begin @ %d sec %d nsec\n", timeOld.tv_sec, timeOld.tv_nsec);
		distance_cm = getDistance();		//checking for any onbstacles
		//clock_gettime(CLOCK_REALTIME, &timeNow);
		//LOG_STDOUT(INFO "getDistance end @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);
		
		if(distance_cm < 1)
			continue;
		if(distance_cm < DISTANCE_THRESHOLD_CM)		
		{
     			if(current_state == RUNNING)
     			{
  				ultrasonic_stop_flag = 1;	//if obstacle is detected and current_state is RUNNING
  				//LOG_STDOUT(INFO "Obstacle detected");
  				//LOG_STDOUT(INFO "Obstacle detected at %dcm. Setting event ULTRASONIC STOP FLAG @ %d sec %d nsec\n", distance_cm,timeNow.tv_sec, timeNow.tv_nsec);
  				//SEMPOST
  				ret = sem_post(&control_task_semaphore);	//semaphore post to control task
  				if(ret)
  				{
  					LOG_STDOUT(ERROR "SEMAPHORE POST FAILED\n");
  					//exit(ERROR);
  					//return ERROR;
  				}
      			}
			//The car control task will set the car_state to STOPPED
		}
		else
		{
			if(current_state == STOPPED)		
			{
				ultrasonic_start_flag = 1;	//if no obstacle is detected and current state is STOPPED
				clock_gettime(CLOCK_REALTIME, &timeNow);
				//LOG_STDOUT(INFO "Obstacle removed\n");
				//LOG_STDOUT(INFO "Obstacle removed (%dcm). Setting event ULTRASONIC START FLAG @ %d sec %d nsec\n", distance_cm, timeNow.tv_sec, timeNow.tv_nsec);
				//SEMPOST
				ret = sem_post(&control_task_semaphore);	//posting semaphore to control task
				if(ret)
				{
					LOG_STDOUT(ERROR "SEMAPHORE POST FAILED\n");
					//exit(ERROR);
					//return ERROR;
				}
				//The car control task will set the car_start to RUNNING
			}
		}
		usleep(300000);
	}

	return SUCCESS;
}
