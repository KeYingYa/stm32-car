
#ifndef __ENCODE_H
#define __ENCODE_H

#include "stm32f1xx_hal.h"  // 根据实际芯片型号调整

typedef struct {
    // 硬件相关参数
    TIM_HandleTypeDef *htim;          // 定时器句柄
    uint32_t last_pulse_count;        // 上次脉冲计数值
    
    // 机械参数
    float gear_ratio;                 // 变速箱减速比
    float pulses_per_motor_rev;       // 电机每转脉冲数
    
    // 运行参数
    float update_period_sec;          // 更新周期(秒)
    
    // 计算结果
    float motor_speed_rpm;            // 电机转速(RPM)
    float wheel_speed_rpm;            // 车轮转速(RPM)
    float wheel_revolutions;          // 车轮总转数(里程)
		uint32_t last_update_tick;  // 添加时间戳跟踪
} Wheel_HandleTypeDef;

extern Wheel_HandleTypeDef wheel1, wheel2;


// 初始化函数
void Wheel_Init(Wheel_HandleTypeDef *wheel, 
               TIM_HandleTypeDef *htim,
               float gear_ratio, 
               float pulses_per_rev);

// 速度计算和里程更新
void WheelSpeed_Calculate(Wheel_HandleTypeDef *wheel);

// OLED显示函数
void Encode_show(Wheel_HandleTypeDef wheel1, Wheel_HandleTypeDef wheel2);

#endif /* __ENCODE_H */
