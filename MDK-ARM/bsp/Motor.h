#ifndef __MOTOR_H
#define __MOTOR_H

#include "tim.h"
#include "gpio.h"
#include "PID.h"
extern PID_t PID_Wheel1;  // 外部声明
extern PID_t PID_Wheel2;  // 外部声明

void SetMotorLeft(float speed);
void SetMotorRight(float speed);
void SetMotor(float speed, float turn);

#endif 
