#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "stm32f1xx_hal.h"  // 根据根据实际芯片型号修改

// 方向定义
typedef enum {
    STEPPER_DIR_CW = 0,     // 顺时针
    STEPPER_DIR_CCW = 1     // 逆时针
} StepperDirection;

// 步进电机结构体
typedef struct {
    TIM_HandleTypeDef* htim;       // 定时器句柄
    uint32_t channel;              // PWM通道
    GPIO_TypeDef* dir_port;        // 方向引脚端口
    uint16_t dir_pin;              // 方向引脚
    uint32_t steps_per_rev;        // 每圈步数
    uint32_t timer_clock;          // 定时器时钟频率(Hz)
    uint16_t current_rpm;          // 当前转速
    StepperDirection current_dir;  // 当前方向
} StepperMotor;

// 函数声明
void Stepper_Init(StepperMotor* motor, TIM_HandleTypeDef* htim, uint32_t channel,
                 GPIO_TypeDef* dir_port, uint16_t dir_pin, 
                 uint32_t steps_per_rev, uint32_t timer_clock);

void Stepper_SetDirection(StepperMotor* motor, StepperDirection dir);
void Stepper_SetSpeed(StepperMotor* motor, uint16_t rpm);
void Stepper_Start(StepperMotor* motor);
void Stepper_Stop(StepperMotor* motor);

#endif

