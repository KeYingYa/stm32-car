#ifndef __LINE_H
#define __LINE_H
#include "i2c.h"
#include "main.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <math.h>
#include "PID.h"
typedef struct {
    // 配置参数
    float positions[8];      // 传感器物理位置（单位：cm）
    float min_threshold;     // 最小检测阈值（0.0-1.0）
    float max_threshold;     // 最大检测阈值（0.0-1.0）
    float center_position;   // 中心位置（预计算）
    
    // 状态变量
    float position;          // 当前黑线位置（单位：cm）
    float error;             // 当前位置误差（单位：cm）
    uint8_t detected;        // 检测状态（1=检测到，0=未检测到）
    
    // 中间计算值
    float normalized[8];     // 归一化后的传感器值
} Line;
extern PID_t PID_Line;
void LineProgress(unsigned short data[8], Line* line);
void LineInit(Line* line, float spacing);
#endif /* __LINE_H */
