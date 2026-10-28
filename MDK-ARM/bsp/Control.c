#include "Control.h"

int Straight(int speed, int mileage) {
    static bool initialized = false;
    static uint32_t wheel1_init = 0;
    static uint32_t wheel2_init = 0;
    static int current_speed = 0;  // 当前实际速度
    const int ACCEL_STEP = 10;      // 加速度步长
    
    // 处理零里程情况
    if (mileage <= 0) {
        if (initialized) {
            SetMotor(0, 0);
            initialized = false;
        }
        return 1;
    }
    
    if (!initialized) {
        wheel1_init = wheel1.wheel_revolutions;
        wheel2_init = wheel2.wheel_revolutions;
        initialized = true;
        current_speed = 0;  // 从0开始加速
        SetMotor(current_speed, 0);  // 初始速度为0
        return 0;
    }
    
    // 缓慢加速到目标速度
    if (current_speed < speed) {
        current_speed += ACCEL_STEP;
        if (current_speed > speed) current_speed = speed;
        SetMotor(current_speed, 0);
    }
    
    uint32_t wheel1_diff = wheel1.wheel_revolutions - wheel1_init;
    uint32_t wheel2_diff = wheel2.wheel_revolutions - wheel2_init;
    
    // 检查是否达到目标里程
    if (wheel1_diff >= (uint32_t)mileage && wheel2_diff >= (uint32_t)mileage) {
        initialized = false;
        SetMotor(0, 0);
        return 1;
    }
    
    return 0;
}



PID_t PID_SelfTurn={
    .Kp = 0.8,  // 增加比例系数
    .Ki = 0,   // 积分系数
    .Kd = 0.3,   // 微分系数
    .OutMax = 100,
    .OutMin = -100,
    .Out = 0
};

uint8_t SelfTurn_flag=0;


bool SelfTurn(float angle) {
    static uint32_t start_time = 0;
    static bool is_active = false;
    
    // 如果函数被调用且当前没有激活
    if (!is_active) {
        // 设置目标角度
        PID_SelfTurn.Target = angle_data.yaw + angle;
        
        // 设置转向标志
        SelfTurn_flag = 1;
        
        // 记录开始时间
        start_time = HAL_GetTick();
        
        // 标记为激活状态
        is_active = true;
        
        return false; // 返回未完成
    }
    
    // 检查是否已等待2秒
    if (HAL_GetTick() - start_time >= 2000) {
        // 清除转向标志
        SelfTurn_flag = 0;
        
        // 重置激活状态
        is_active = false;
        
        return true; // 返回已完成
    }
    
    return false; // 仍在等待中
}



