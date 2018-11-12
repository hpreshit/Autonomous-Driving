

#ifndef COMMON_HELPER_H
#define COMMON_HELPER_H

#include "posixTimer.h"

typedef enum
{
	RUNNING = 0,
	STOPPED,
	CAMERA_STOPPED,
	SPEED_SLOW,
}CAR_STATE;

/*
static int register_and_start_timer(timer_t *timer_id, uint32_t usec, uint8_t oneshot, void (*timer_handler)(union sigval), void *handlerArgs)
{
  if(register_timer(timer_id, timer_handler, timer_id) == -1)
	{
	    LOG_STDOUT("[ERROR] Register Timer\n");
		return ERR;
	}
	// else
	// 	LOG_STDOUT("[INFO] Timer created\n");
	
	if(start_timer(*timer_id , usec, oneshot) == -1)
	{
		LOG_STDOUT("[ERROR] Start Timer\n");
		return ERR;
	}
	// else
	// 	LOG_STDOUT("[INFO] Timer started\n");

}
*/

#endif

