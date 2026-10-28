#include "main.h"
#include "gpio.h"
#include "key.h"
#include "imu.h"
uint8_t key_num = 0;
extern SetZero set_zero;

volatile uint8_t key_pressed = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_3) 
    {
        // 只设置标志位，不进行任何处理
        key_pressed = 1;
    }
}

void Process_Key(void)
{
    static uint8_t key_state = 0;
    
    if (key_pressed)
    {
        key_pressed = 0;  // 清除标志
        
        // 第一步：防抖延时
        HAL_Delay(20);  // 20ms防抖延时
        
        // 再次确认按键状态（防止抖动）
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_PIN_3) == GPIO_PIN_SET) // 根据实际硬件调整
        {
            // 第二步：执行按键操作
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
            speed += 20;
            
            // 第三步：切换IMU零位设置
            if (imu_key == 0)
            {
                imu_setzero(accel_data, gyro_data, angle_data, &set_zero);
                imu_key = 1;
            }
            else
            {
                // 重置零位
                set_zero.ax_zero = 0.0f;
                set_zero.ay_zero = 0.0f;
                set_zero.az_zero = 0.0f;
                set_zero.wx_zero = 0.0f;
                set_zero.wy_zero = 0.0f;
                set_zero.wz_zero = 0.0f;
                imu_key = 0;
            }
        }
    }
}