#include <wiringPi.h>
#include <softPwm.h>
#include "motordriver.h"
#include <unistd.h>
/************Preshit**************/

#define ON    40
#define OFF   0

#define RANGE    100      //pwmRange
#define INITIAL_VALUE    0 

#define MOTOR1_A    0    //PI pin - 11
#define MOTOR1_B    1    //PI pin - 12
#define MOTOR2_A    3    //PI pin - 15
#define MOTOR2_B    4    //PI pin - 16
#define MOTOR1_EN   12 	 //PI pin - 19
#define MOTOR2_EN   13	 //PI pin - 21

void Motor_Setup()
{
	wiringPiSetup();
  
 	pinMode(MOTOR1_A, OUTPUT);
	pinMode(MOTOR1_B, OUTPUT);
	pinMode(MOTOR2_A, OUTPUT);
	pinMode(MOTOR2_B, OUTPUT);
  
	pinMode(MOTOR1_EN, OUTPUT);
	pinMode(MOTOR2_EN, OUTPUT);
	digitalWrite(MOTOR1_EN,HIGH);
	digitalWrite(MOTOR2_EN,HIGH);

	//left motor inititalized
	softPwmCreate(MOTOR1_A, INITIAL_VALUE, RANGE);
	softPwmCreate(MOTOR1_B, INITIAL_VALUE, RANGE);

	//right motor initialized
	softPwmCreate(MOTOR2_A, INITIAL_VALUE, RANGE);
	softPwmCreate(MOTOR2_B, INITIAL_VALUE, RANGE);

	Motor_Start();
	sleep(2);
	Motor_Reverse();
	sleep(1);
	Motor_Stop();
}


void Motor_Start()
{
	softPwmWrite(MOTOR1_A, ON);
	softPwmWrite(MOTOR2_A, ON);
	softPwmWrite(MOTOR1_B, OFF);
	softPwmWrite(MOTOR2_B, OFF);
}

void Motor_Stop()
{
	softPwmWrite(MOTOR1_A, OFF);
	softPwmWrite(MOTOR2_B, OFF);
	softPwmWrite(MOTOR1_B, OFF);
	softPwmWrite(MOTOR2_A, OFF);
}

void Motor_Reverse()
{
	softPwmWrite(MOTOR1_A, OFF);
	softPwmWrite(MOTOR2_A, OFF);
	softPwmWrite(MOTOR1_B, ON);
	softPwmWrite(MOTOR2_B, ON);
}
