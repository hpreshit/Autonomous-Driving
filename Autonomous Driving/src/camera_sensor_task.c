

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#include "camera_sensor_task.h"
#include "utilities.h"
#include "common_helper.h"
#include "camera_stopsign.h"

#define RADIUS_THRESHOLD 50

extern uint8_t current_state;
extern uint8_t camera_stop_flag;
extern sem_t control_task_semaphore;
extern uint8_t stop_sign_detected;

/*************camera sensor task()***********/
//STOP sign detection done
//Check for distance of the vehicle from STOP sign
//Signal the control task based on the detection and current state of the vehicle
/********************************************/
void* camera_sensor_task(void* params)
{
	struct timespec timeNow;
#ifdef CONTROL_TASK_TEST 
 	while(1)
  	{
		sleep(10);
		camera_stop_flag = 1;
		clock_gettime(CLOCK_REALTIME, &timeNow);
		LOG_STDOUT(INFO "STOP SIGN detected\n");  
	  	//LOG_STDOUT(INFO "STOP SIGN detected. Setting event CAMERA STOP FLAG @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);  
		uint8_t ret = sem_post(&control_task_semaphore);
		if(ret)
		{
			LOG_STDOUT(ERROR "SEMAPHORE POST FAILED\n");
			//exit(ERROR);
			//return ERROR;
		}
	}

#else
	if(camera_init() == -1)
	{
		LOG_STDOUT(ERROR "Camera init failed\n");
		return ERR;
	}
	int radius = -1;	
	while(1)
	{
		
    		//clock_gettime(CLOCK_REALTIME, &timeNow);
	  	//LOG_STDOUT(INFO "CAM PROC started @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);  
		radius = getStopSignRadius();		//returns radius of the stop sign detected
    		//clock_gettime(CLOCK_REALTIME, &timeNow);
	  	//LOG_STDOUT(INFO "CAM PROC ended @ %d sec %d nsec\n", timeNow.tv_sec, timeNow.tv_nsec);  
		//printf("Rad: %d\n",radius);
		if(!stop_sign_detected && radius > RADIUS_THRESHOLD && current_state != CAMERA_STOPPED && current_state != STOPPED)
		{
			camera_stop_flag = 1;	
    			clock_gettime(CLOCK_REALTIME, &timeNow);
			LOG_STDOUT(INFO "STOP SIGN detected. RAD:%d\n",radius);  
	  		//LOG_STDOUT(INFO "STOP SIGN detected. RAD:%d. Setting event CAMERA STOP FLAG @ %d sec %d nsec\n", radius, timeNow.tv_sec, timeNow.tv_nsec);  
  			uint8_t ret = sem_post(&control_task_semaphore);	//semaphore post to control task if radius is above threshold
  			if(ret)
  			{
  				LOG_STDOUT(ERROR "SEMAPHORE POST FAILED\n");
  				//exit(ERROR);
  			}
		}
	}


#endif	
	return SUCCESS;
}
