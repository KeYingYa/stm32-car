#ifndef __IMU_H
#define __IMU_H
#include "stdbool.h"
typedef struct {
    float ax;
    float ay;
    float az;
} AccelData;

typedef struct {
    float wx;
    float wy;
    float wz;
} GyroData;

typedef struct {
    float roll;
    float pitch;
    float yaw;
	  float unwrapped_yaw;     // 解缠绕后的连续yaw
    float last_raw_yaw;      // 上一次的原始yaw（-180~180范围）
    bool is_first_yaw;       // 首次接收标志
} AngleData;

typedef struct {
    // 加速度计零偏
    float ax_zero;
    float ay_zero;
    float az_zero;
    // 陀螺仪零偏
    float wx_zero;
    float wy_zero;
    float wz_zero;
} SetZero;

extern AccelData accel_data;
extern GyroData gyro_data;
extern AngleData angle_data;

void SendReadAccelCommand(void);
void ProcessReceivedData(uint8_t *buffer, uint16_t size, AccelData *accel_data, GyroData *gyro_data, AngleData *angle_data,SetZero*setzero)  ;
void imu_show(AccelData accel_data, GyroData gyro_data, AngleData angle_data,uint8_t show_num);
void imu_setzero(AccelData accel_data, GyroData gyro_data, AngleData angle_data,SetZero *setzero);
#endif
