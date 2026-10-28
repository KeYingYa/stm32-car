#include "Motor.h"
#include "tim.h"
#include "gpio.h"
#include "main.h"
#include "Encode.h"

// 初始化PID参数
PID_t PID_Wheel1 = {
    .Kp = 0.6,  // 斩輷覉}系私
    .Ki = 0.1,   // 芙貣系私
    .Kd = 0.3,   // 微貣系私
    .OutMax = 100,
    .OutMin = 0,
    .Out = 0
};


PID_t PID_Wheel2 = {
    .Kp = 0.6,  // 增加比例系数
    .Ki = 0.1,   // 积分系数
    .Kd = 0.3,   // 微分系数
    .OutMax = 100,
    .OutMin = 0,
    .Out = 0
};


extern Wheel_HandleTypeDef wheel1, wheel2;

void SetMotorRight(float speed)
{
    // 计算目标速度的绝对值（用于PID控制）
    float target_speed = (speed >= 0) ? speed : -speed;
    
    // 设置电机方向控制引脚
    // 使用H桥驱动时，两个引脚不能同时为高电平
    if (speed < 0) {
        // 正转：IN1=低电平，IN2=高电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    } 
    else if (speed > 0) {
        // 反转：IN1=高电平，IN2=低电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
    } 
    
    // 设置PID控制器的目标速度
    PID_Wheel1.Target = target_speed;
}

// 设置左电机速度和方向
void SetMotorLeft(float speed)
{
    // 计算目标速度的绝对值（用于PID控制）
    float target_speed = (speed >= 0) ? speed : -speed;
    
    // 设置电机方向控制引脚
    if (speed < 0) {
        // 正转：IN1=高电平，IN2=低电平
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    } 
    else if (speed > 0) {
        // 反转：IN1=低电平，IN2=高电平
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    } 
    
    // 设置PID控制器的目标速度
    PID_Wheel2.Target = target_speed;
}

void SetMotor(float speed, float turn)
{
	
	float leftSpeed, rightSpeed;

    if (turn > 0) {
        leftSpeed = speed;
        rightSpeed = speed * (1 - turn);
    } else {
        rightSpeed = speed;
        leftSpeed = speed * (1 + turn);
    }

    SetMotorLeft(leftSpeed);
    SetMotorRight(rightSpeed);
}


