
#include <wiringPi.h>
#include <stdint.h>

/*Connect the below pins*/
/***********************
DRIVER_PIN 	- 	PI_PIN
12			-	29
4			-	31
7			-	33
8			-	35
11			-	37
***********************/

/**
MOTORNUM = 1,2,3,4
TESTED = Motornum 1 and 2 only
**/
 
typedef enum motor_cmd
{
	FORWARD,
	REVERSE,
	STOP
}MOTOR_CMD_T;


//void DCMotor_enable(uint8_t motornum);
//void DC_motor_run(uint8_t motornum, MOTOR_CMD_T cmd);
//void DC_motor_run_two(uint8_t motornum1, uint8_t motornum2, MOTOR_CMD_T cmd);

void Motor_Setup();
void Motor_Stop();
void Motor_Start();
void Motor_Reverse();
