#include "Line.h"

void LineProgress(unsigned short data[8], Line* line) {
    // 1. 数据反转（最大白值4096）
    uint16_t reversed[8] = {
        4096 - data[0],
        4096 - data[1],
        4096 - data[2],
        4096 - data[3],
        4096 - data[4],
        4096 - data[5],
        4096 - data[6],
        4096 - data[7]
    };
    
    // 新增：检查所有值是否都大于3000（都在白色区域）
//    uint8_t all_white = 1;
//    for (int i = 0; i < 8; i++) {
//        if (data[i] <= 2000) {
//            all_white = 0;
//            break;
//        }
//    }
//    
//    if (all_white) {
//        line->detected = 0;
//        line->position = -999.0f;
//        line->error = 0.0f;
//        return;
//    }
    
    // 2. 寻找最小值和最大值
    uint16_t min_val = 4096;
    uint16_t max_val = 0;
    for (int i = 0; i < 8; i++) {
        if (reversed[i] < min_val) min_val = reversed[i];
        if (reversed[i] > max_val) max_val = reversed[i];
    }
    
    // 3. 计算有效检测范围
    uint16_t range = max_val - min_val;
    if (range < 5) {  // 范围太小视为无效检测
        line->detected = 0;
        line->position = -999.0f;
        line->error = 0.0f;
        return;
    }
    
    // 4. 归一化处理
    float weighted_sum = 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < 8; i++) {
        // 归一化到0-1范围
        line->normalized[i] = (float)(reversed[i] - min_val) / range;
        
        // 应用阈值处理
        if (line->normalized[i] < line->min_threshold) 
            line->normalized[i] = 0.0f;
        else if (line->normalized[i] > line->max_threshold) 
            line->normalized[i] = 1.0f;
        
        // 增强响应（平方处理）
        line->normalized[i] = line->normalized[i] * line->normalized[i];
        
        // 计算加权位置
        weighted_sum += line->normalized[i] * line->positions[i];
        sum += line->normalized[i];
    }
    
    // 5. 计算中心位置
    if (sum > 0.1f) {
        line->detected = 1;
        line->position = weighted_sum / sum;
        line->error = line->position - line->center_position;
    } else {
        line->detected = 0;
        line->position = -999.0f;
        line->error = 0.0f;
    }
}

// 初始化传感器配置
void LineInit(Line* line, float spacing) {
    // 设置传感器位置（从左到右）
    for (int i = 0; i < 8; i++) {
        line->positions[i] = i * spacing;
    }
    
    // 计算中心位置（传感器阵列中心）
    line->center_position = (8 - 1) * spacing / 2.0f;
    
    // 设置默认阈值
    line->min_threshold = 0.2f;
    line->max_threshold = 0.8f;
    
    // 初始化状态
    line->position = -999.0f;
    line->error = 0.0f;
    line->detected = 0;
}

// PID控制器配置
PID_t PID_Line = {
    .Target = 0,
    .Kp = 0.25f,  // 比例系数
    .Ki = 0.0f, // 积分系数
    .Kd = 0.65f, // 微分系数
    .OutMax = 1.0f,
    .OutMin = -1.0f,
    .Out = 0.0f
	
	
};


