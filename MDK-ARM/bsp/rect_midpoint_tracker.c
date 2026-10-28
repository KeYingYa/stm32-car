#include "rect_midpoint_tracker.h"
#include <stddef.h>
#include <string.h>

// 定义常量
#define INVALID_FRAME_HEADER_1 0xAA
#define INVALID_FRAME_HEADER_2 0xBB
#define VALID_FRAME_HEADER_1 0x55
#define VALID_FRAME_HEADER_2 0x56
#define MIN_FRAME_LENGTH 6  // 最小有效帧长度(2字节头+2字节x+2字节y)

// 全局结果存储
RectMidpointResult rectResults;

/**
 * 解析矩形中点数据帧，获取矩形中点的位置信息
 * @param data 接收到的数据帧
 * @param length 数据长度
 * @param result 存储解析结果的结构体指针
 * @return 解析成功返回 true，失败返回 false
 */
bool ParseRectMidpoints(const uint8_t *data, uint8_t length, RectMidpointResult *result) {
    // 参数有效性检查
    if (data == NULL || result == NULL || length < MIN_FRAME_LENGTH) {
        result->valid = false;
        return false;
    }

    // 初始化结果为无效
    result->valid = false;
    
    // 遍历缓冲区寻找有效帧
    for (uint16_t i = 0; i <= length - MIN_FRAME_LENGTH; i++) {
        // 检查是否为无效帧 (AA BB)
        if (i <= length - 2 && data[i] == INVALID_FRAME_HEADER_1 && data[i + 1] == INVALID_FRAME_HEADER_2) {
            // 跳过无效帧
            i += 1; // 前进一个字节继续搜索
            continue;
        }
        
        // 检查有效帧头 (55 56)
        if (data[i] == VALID_FRAME_HEADER_1 && data[i + 1] == VALID_FRAME_HEADER_2) {
            // 确保有足够的数据解析完整帧
            if (i + MIN_FRAME_LENGTH > length) {
                return false; // 数据不足
            }
            
            // 解析X和Y坐标（大端格式）
            result->x = ((uint16_t)data[i + 2] << 8) | data[i + 3];
            result->y = ((uint16_t)data[i + 4] << 8) | data[i + 5];
            
            // 标记为有效
            result->valid = true;
            
            // 可选：添加CRC校验
            /*
            uint8_t crc = 0;
            for (int j = 0; j < 6; j++) {
                crc ^= data[i + j];
            }
            if (crc != 0) {
                result->valid = false;
                continue; // CRC校验失败，继续搜索
            }
            */
            
            return true; // 成功解析一帧
        }
    }
    
    return false; // 未找到有效帧
}