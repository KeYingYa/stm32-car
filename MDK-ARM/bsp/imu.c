#include "main.h"
#include "usart.h"
#include "imu.h"
#include "oled.h"
#include "stdio.h"

uint8_t imu_flag;

AccelData accel_data;
GyroData gyro_data;
AngleData angle_data;

void Unlock_Register(void)
{
    uint8_t command[5] = {0xFF, 0xAA, 0x69, 0x88, 0xB5};
    HAL_UART_Transmit(&huart3, command, sizeof(command),10);
}
void SaveSettings(void)
{
    uint8_t command[5] = {0xFF, 0xAA, 0x00, 0x00, 0x00};
    HAL_UART_Transmit(&huart3, command, sizeof(command),10);
}

void ReadAcceleration(void)
{
    uint8_t command[5] = {0xFF, 0xAA, 0x27, 0x34, 0x00};
    HAL_UART_Transmit(&huart3, command, sizeof(command),10);

}

void ReadAngularVelocity(void)
{
    uint8_t command[5] = {0xFF, 0xAA, 0x27, 0x37, 0x00};
    HAL_UART_Transmit(&huart3, command, sizeof(command),10);

}

void ReadAngule(void)
{
    uint8_t command[5] = {0xFF, 0xAA, 0x27, 0x3D, 0x00};
    HAL_UART_Transmit(&huart3, command, sizeof(command),10);

}

void SendReadAccelCommand(void)
{
	Unlock_Register();
	HAL_Delay(10);
	ReadAcceleration();
	HAL_Delay(10);
	SaveSettings();
}

void ProcessReceivedData(uint8_t *buffer, uint16_t size, AccelData *accel_data, GyroData *gyro_data, AngleData *angle_data,SetZero *setzero) 
	{
    for (int i = 0; i < size - 10; i++) {
        if (buffer[i] == 0x55) { // 找到帧头
            switch (buffer[i + 1]) { // 判断数据类型
                case 0x51: // 加速度数据
                {
                    int16_t ax = (buffer[i + 3] << 8) | buffer[i + 2];
                    int16_t ay = (buffer[i + 5] << 8) | buffer[i + 4];
                    int16_t az = (buffer[i + 7] << 8) | buffer[i + 6];
                    
                    const float accel_scale = 16.0f * 9.8f / 32768.0f;
                    accel_data->ax = ax * accel_scale-setzero->ax_zero;
                    accel_data->ay = ay * accel_scale-setzero->ay_zero;
                    accel_data->az = az * accel_scale-setzero->az_zero;
                    break;
                }
                case 0x52: // 角速度数据
                {
                    int16_t wx = (buffer[i + 3] << 8) | buffer[i + 2];
                    int16_t wy = (buffer[i + 5] << 8) | buffer[i + 4];
                    int16_t wz = (buffer[i + 7] << 8) | buffer[i + 6];
                    
                    const float gyro_scale = 2000.0f / 32768.0f;
                    gyro_data->wx = wx * gyro_scale-setzero->wx_zero;
                    gyro_data->wy = wy * gyro_scale-setzero->wy_zero;
                    gyro_data->wz = wz * gyro_scale-setzero->wz_zero;
                    break;
                }

								
								case 0x53: // 角度数据
								{
    int16_t roll = (buffer[i + 3] << 8) | buffer[i + 2];
    int16_t pitch = (buffer[i + 5] << 8) | buffer[i + 4];
    int16_t yaw = (buffer[i + 7] << 8) | buffer[i + 6];
    
    const float angle_scale = 180.0f / 32768.0f;
    angle_data->roll = roll * angle_scale;
    angle_data->pitch = pitch * angle_scale;
    float current_raw_yaw = yaw * angle_scale;  // 当前原始yaw（-180~180）

    // 偏航角解缠绕处理
    if (angle_data->is_first_yaw) {
        // 第一次接收：初始化状态
        angle_data->unwrapped_yaw = current_raw_yaw;
        angle_data->last_raw_yaw = current_raw_yaw;
        angle_data->is_first_yaw = false;
    } else {
        // 计算与上一次的角度差
        float delta = current_raw_yaw - angle_data->last_raw_yaw;
        
        // 检测并修正360°跳变
        if (delta > 180.0f) {
            delta -= 360.0f;  // 正向跳变修正（例如179° → -181°）
        } else if (delta < -180.0f) {
            delta += 360.0f; // 负向跳变修正（例如-179° → 181°）
        }
        
        // 更新解缠绕后的连续角度
        angle_data->unwrapped_yaw += delta;
        angle_data->last_raw_yaw = current_raw_yaw;  // 更新原始角度记录
    }
    
    // 使用解缠绕后的yaw（可根据需求替换原始yaw）
    angle_data->yaw = angle_data->unwrapped_yaw; 
    break;
}
                default:
                    break;
            }
        }
    }
}

void imu_show(AccelData accel_data, GyroData gyro_data, AngleData angle_data,uint8_t show_num)
{
	   char ax_str[20], ay_str[20], az_str[20];
    char wx_str[20], wy_str[20], wz_str[20];
    char roll_str[20], pitch_str[20], yaw_str[20];
    sprintf(ax_str, "ax:%.2f", accel_data.ax);
    sprintf(ay_str, "ay:%.2f", accel_data.ay);
    sprintf(az_str, "az:%.2f", accel_data.az);
    sprintf(wx_str, "wx:%.2f", gyro_data.wx);
    sprintf(wy_str, "wy:%.2f", gyro_data.wy);
    sprintf(wz_str, "wz:%.2f", gyro_data.wz);
    sprintf(roll_str, "roll:%.2f", angle_data.roll);
    sprintf(pitch_str, "pitch:%.2f", angle_data.pitch);
    sprintf(yaw_str, "yaw:%.2f", angle_data.yaw);

		if(show_num==1)
		{
    OLED_PrintString(0, 16, ax_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 32, ay_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 48, az_str, &font16x16, OLED_COLOR_NORMAL);		
		}
		else if(show_num==2)
		{
    OLED_PrintString(0, 16, wx_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 32, wy_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 48, wz_str, &font16x16, OLED_COLOR_NORMAL);				
		}
		else if(show_num==3)
		{
    OLED_PrintString(0, 16, roll_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 32, pitch_str, &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 48, yaw_str, &font16x16, OLED_COLOR_NORMAL);				
		}		
;
		
}

void imu_setzero(AccelData accel_data, GyroData gyro_data, AngleData angle_data,SetZero *setzero)
{
		float accel_std_z = 9.8f;
    setzero->ax_zero = accel_data.ax;
    setzero->ay_zero = accel_data.ay;
    setzero->az_zero = accel_data.az - accel_std_z;

    // 陀螺仪在校零时应该静止，数据应该接近 0
    setzero->wx_zero = gyro_data.wx;
    setzero->wy_zero = gyro_data.wy;
    setzero->wz_zero = gyro_data.wz;


}
