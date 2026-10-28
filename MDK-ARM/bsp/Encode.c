
#include "tim.h"
#include "gpio.h"
#include "stdlib.h"
#include "math.h"
#include "main.h"
#include "stdio.h"
#include "oled.h"
#include "Encode.h"

#define DEFAULT_UPDATE_PERIOD 0.01f  // 默认1ms更新周期
#define MIN_PULSES_PER_REV 0.1f       // 最小有效脉冲数阈值
#define EPSILON 1e-5f                // 浮点数比较阈值
Wheel_HandleTypeDef wheel1, wheel2;

void Wheel_Init(Wheel_HandleTypeDef *wheel, 
               TIM_HandleTypeDef *htim,
               float gear_ratio, 
               float pulses_per_rev)
{
    if(!wheel || !htim) return;
    
    wheel->htim = htim;
    wheel->gear_ratio = (gear_ratio > 0) ? gear_ratio : 1.0f;
    wheel->pulses_per_motor_rev = (pulses_per_rev > 0) ? pulses_per_rev : 1.0f;
    
    wheel->last_pulse_count = __HAL_TIM_GET_COUNTER(wheel->htim);
    wheel->motor_speed_rpm = 0.0f;
    wheel->wheel_speed_rpm = 0.0f;
    wheel->wheel_revolutions = 0.0f;
    wheel->update_period_sec = DEFAULT_UPDATE_PERIOD;
}

void WheelSpeed_Calculate(Wheel_HandleTypeDef *wheel)
{
    if (!wheel || !wheel->htim) return;

    // 1. 计算实际时间差（毫秒）
    uint32_t current_tick = HAL_GetTick();
    uint32_t elapsed_ms = current_tick - wheel->last_update_tick;
    wheel->last_update_tick = current_tick;
    
    // 2. 确保最小时间阈值（防止除零）
    if(elapsed_ms < 1) elapsed_ms = 1;
    float elapsed_sec = elapsed_ms / 1000.0f;

    // 3. 获取脉冲差值（修复方向）
    uint32_t current_count = __HAL_TIM_GET_COUNTER(wheel->htim);
    const uint32_t timer_period = __HAL_TIM_GET_AUTORELOAD(wheel->htim) + 1;
    const int32_t half_range = (int32_t)timer_period / 2;
    
    // 修复：反转脉冲差计算顺序以修正方向
    int32_t delta_pulses = (int32_t)(wheel->last_pulse_count - current_count); // 顺序反转
    
    // 溢出处理（保持不变）
    if (delta_pulses > half_range) delta_pulses -= timer_period;
    else if (delta_pulses < -half_range) delta_pulses += timer_period;
    
    wheel->last_pulse_count = current_count;

    // 4. 精确转速计算（使用实际时间差）
    if (wheel->pulses_per_motor_rev > MIN_PULSES_PER_REV) 
    {
        // 计算电机转速（方向已修正）
        float pulses_per_second = (float)delta_pulses / elapsed_sec;
        wheel->motor_speed_rpm = (pulses_per_second * 60.0f) / wheel->pulses_per_motor_rev;
        
        // 计算轮速
        wheel->wheel_speed_rpm = wheel->motor_speed_rpm / wheel->gear_ratio;
    }
    else {
        wheel->motor_speed_rpm = 0.0f;
        wheel->wheel_speed_rpm = 0.0f;
    }

    // 5. 里程累积（使用绝对值保持正增长）
    float wheel_revs = fabsf((float)delta_pulses) / 
                     (wheel->pulses_per_motor_rev * wheel->gear_ratio);
    wheel->wheel_revolutions += wheel_revs;
}

void Encode_show(Wheel_HandleTypeDef wheel1, Wheel_HandleTypeDef wheel2)
{
    char buffer[4][32];  // 4行显示缓冲区
    
    snprintf(buffer[0], sizeof(buffer[0]), "S1:%.2f RPM", wheel1.wheel_speed_rpm);
    snprintf(buffer[1], sizeof(buffer[1]), "D1:%.3f rev", wheel1.wheel_revolutions);
    snprintf(buffer[2], sizeof(buffer[2]), "S2:%.2f RPM", wheel2.wheel_speed_rpm);
    snprintf(buffer[3], sizeof(buffer[3]), "D2:%.3f rev", wheel2.wheel_revolutions);
    
    for(uint8_t i = 0; i < 4; i++) {
        OLED_PrintString(0, i*16, buffer[i], &font16x16, OLED_COLOR_NORMAL);
    }
}






