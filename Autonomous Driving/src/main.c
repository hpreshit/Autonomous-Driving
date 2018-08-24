
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>

#include "sonar_sensor_task.h"
#include "control_task.h"
#include "camera_sensor_task.h"
#include "utilities.h"

typedef enum
{
	MOTOR_CONTROL_SERVICE = 0,
	SONAR_SENSOR_SERVICE,
	CAMERA_SERVICE,
	LAST_SERVICE		//This should not be used, this is just to count the number of services
}THREAD_ID;

#define NUM_THREADS 			LAST_SERVICE

void* (*thread_callbacks[NUM_THREADS])(void *) =
{
	control_task,
	sonar_sensor_task,
	camera_sensor_task
};

const char * const SERVICE_NAME[NUM_THREADS] =
{
	(const char *const)"MOTOR_CONTROL_SERVICE",
	(const char *const)"SONAR_SENSOR_SERVICE",
	(const char *const)"CAMERA_SERVICE",
};

pthread_t threads[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
pthread_attr_t main_attr;
int rt_max_prio, rt_min_prio;
struct sched_param rt_param[NUM_THREADS];
int rt_protocol;

int main()
{
	struct timespec timeNow;
	int ret;
	
	print_scheduler();
	
	int scope;
	pthread_attr_getscope(&main_attr, &scope);

 	if(scope == PTHREAD_SCOPE_SYSTEM)
    		printf("PTHREAD SCOPE SYSTEM\n");
  	else if (scope == PTHREAD_SCOPE_PROCESS)
    		printf("PTHREAD SCOPE PROCESS\n");
	else
	    	printf("PTHREAD SCOPE UNKNOWN\n");
	

	
	pthread_attr_init(&rt_sched_attr[MOTOR_CONTROL_SERVICE]);
	pthread_attr_setinheritsched(&rt_sched_attr[MOTOR_CONTROL_SERVICE], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr[MOTOR_CONTROL_SERVICE], SCHED_FIFO);
	pthread_attr_setscope(&rt_sched_attr[MOTOR_CONTROL_SERVICE], PTHREAD_SCOPE_SYSTEM);

	pthread_attr_init(&rt_sched_attr[SONAR_SENSOR_SERVICE]);
	pthread_attr_setinheritsched(&rt_sched_attr[SONAR_SENSOR_SERVICE], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr[SONAR_SENSOR_SERVICE], SCHED_FIFO);
	pthread_attr_setscope(&rt_sched_attr[SONAR_SENSOR_SERVICE], PTHREAD_SCOPE_SYSTEM);
	
	pthread_attr_init(&rt_sched_attr[CAMERA_SERVICE]);
	pthread_attr_setinheritsched(&rt_sched_attr[CAMERA_SERVICE], PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr[CAMERA_SERVICE], SCHED_FIFO);
	pthread_attr_setscope(&rt_sched_attr[CAMERA_SERVICE], PTHREAD_SCOPE_SYSTEM);
	
	pthread_attr_getscope(&rt_sched_attr[CAMERA_SERVICE], &scope);

 	if(scope == PTHREAD_SCOPE_SYSTEM)
    		printf("PTHREAD SCOPE SYSTEM\n");
  	else if (scope == PTHREAD_SCOPE_PROCESS)
    		printf("PTHREAD SCOPE PROCESS\n");
	else
	    	printf("PTHREAD SCOPE UNKNOWN\n");
	
	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
	rt_min_prio = sched_get_priority_min(SCHED_FIFO);
	
	struct sched_param params;
	params.sched_priority = rt_max_prio;
	ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &params);
	if (ret != 0) 
	{
		printf("Unsuccessful in setting thread realtime prio\n");
		return 1;     
	}
	
	//Assigning highest priority to motor control service
	rt_param[MOTOR_CONTROL_SERVICE].sched_priority = rt_max_prio -1;
	pthread_attr_setschedparam(&rt_sched_attr[MOTOR_CONTROL_SERVICE], &rt_param[MOTOR_CONTROL_SERVICE]);
	
	//Assigning second highest priority to sonar sensor service
	rt_param[SONAR_SENSOR_SERVICE].sched_priority = rt_max_prio - 2;
	pthread_attr_setschedparam(&rt_sched_attr[SONAR_SENSOR_SERVICE], &rt_param[SONAR_SENSOR_SERVICE]);
	
	//Assigning third highest priority to camera service
	rt_param[CAMERA_SERVICE].sched_priority = rt_max_prio -3;
	pthread_attr_setschedparam(&rt_sched_attr[CAMERA_SERVICE], &rt_param[CAMERA_SERVICE]);

	print_scheduler();
 
   	int i =0;
	/* Create all the child threads */
    	for(i = 0; i < NUM_THREADS; i++)
    	{
        	ret = pthread_create(&threads[i],&rt_sched_attr[i],thread_callbacks[i],NULL);
        	if(ret != 0)
        	{
            		LOG_STDOUT(ERROR "Serivce %s create failed. Reason: %s\n",SERVICE_NAME[i],strerror(errno));
            		return ret;
        	}
    		else
    		{
    			clock_gettime(CLOCK_REALTIME, &timeNow);
    			LOG_STDOUT(INFO "Serivce %s created at %d sec, %d nsec\n",SERVICE_NAME[i],timeNow.tv_sec, timeNow.tv_nsec);
    		}
    	}
	
	
	for(i = 0; i < NUM_THREADS; i++)
    	{
        	int retThread = 0;
        	ret = pthread_join(threads[i],(void**)&retThread);
        	if(ret  != 0)
        	{
			LOG_STDOUT(ERROR "Serivce %s join failed. Reason: %s\n",SERVICE_NAME[i],strerror(errno));
            		return ret;
        	}
		else
		{
			clock_gettime(CLOCK_REALTIME, &timeNow);
			LOG_STDOUT(INFO "Serivce %s exited at %d sec, %d nsec\n",SERVICE_NAME[i],timeNow.tv_sec, timeNow.tv_nsec);	
		}
    	}

	LOG_STDOUT(INFO "GOODBYE\n");

	return 0;
}
