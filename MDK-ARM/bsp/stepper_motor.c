#include "stepper_motor.h"

/**
 * @brief 初始化步进电机
 * @param motor: 电机结构体指针
 * @param htim: 定时器句柄
 * @param channel: PWM通道
 * @param dir_port: 方向引脚端口
 * @param dir_pin: 方向引脚
 * @param steps_per_rev: 每圈步数
 * @param timer_clock: 定时器时钟频率(Hz)
 */
void Stepper_Init(StepperMotor* motor, TIM_HandleTypeDef* htim, uint32_t channel,
                 GPIO_TypeDef* dir_port, uint16_t dir_pin, 
                 uint32_t steps_per_rev, uint32_t timer_clock) {
    motor->htim = htim;
    motor->channel = channel;
    motor->dir_port = dir_port;
    motor->dir_pin = dir_pin;
    motor->steps_per_rev = steps_per_rev;
    motor->timer_clock = timer_clock;
    motor->current_rpm = 0;
    motor->current_dir = STEPPER_DIR_CW;
    
    // 初始化方向引脚为低电平
    HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_RESET);
}

/**
 * @brief 设置步进电机方向
 * @param motor: 电机结构体指针
 * @param dir: 方向(STEPPER_DIR_CW/STEPPER_DIR_CCW)
 */
void Stepper_SetDirection(StepperMotor* motor, StepperDirection dir) {
    motor->current_dir = dir;
    // 设置方向引脚电平
    if (dir == STEPPER_DIR_CW) {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_SET);
    }
}

/**
 * @brief 设置步进电机速度
 * @param motor: 电机结构体指针
 * @param rpm: 目标转速(RPM)
 */
void Stepper_SetSpeed(StepperMotor* motor, uint16_t rpm) {
    if (rpm == 0) {
        Stepper_Stop(motor);
        return;
    }
    
    motor->current_rpm = rpm;
    
    // 计算所需脉冲频率: 频率 = (rpm * 每圈步数) / 60
    uint32_t pulse_freq = (uint32_t)rpm * motor->steps_per_rev / 60;
    
    // 计算PSC和ARR值
    // 为简化计算，固定PSC使定时器时钟为1MHz
    uint32_t psc = (motor->timer_clock / 1000000) - 1;
    uint32_t arr = (1000000 / pulse_freq) - 1;
    
    // 限制ARR最大值(16位定时器最大65535)
    if (arr > 65535) {
        arr = 65535;
    }
    
    // 更新定时器参数
    __HAL_TIM_SET_PRESCALER(motor->htim, psc);
    __HAL_TIM_SET_AUTORELOAD(motor->htim, arr);
    
    // 设置50%占空比
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, arr / 2);
}

/**
 * @brief 启动步进电机
 * @param motor: 电机结构体指针
 */
void Stepper_Start(StepperMotor* motor) {
    if (motor->current_rpm > 0) {
        HAL_TIM_PWM_Start(motor->htim, motor->channel);
    }
}

/**
 * @brief 停止步进电机
 * @param motor: 电机结构体指针
 */
void Stepper_Stop(StepperMotor* motor) {
    HAL_TIM_PWM_Stop(motor->htim, motor->channel);
}
    