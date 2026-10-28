#ifndef __CONTROL_H
#define __CONTROL_H

#include "Motor.h"
#include "Encode.h"
#include "imu.h"
#include "main.h"
#include "stdbool.h"
#include "imu.h"
#include "math.h"
#include "oled.h"
extern PID_t PID_SelfTurn;
extern uint8_t SelfTurn_flag;
int Straight(int speed, int mileage);
bool SelfTurn( float angle);

#endif
