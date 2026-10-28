#ifndef RECT_MIDPOINT_TRACKER_H
#define RECT_MIDPOINT_TRACKER_H

#include <stdint.h>
#include <stdbool.h>

// 矩形中点结果结构体
typedef struct {
    uint16_t x;     // X 坐标
    uint16_t y;     // Y 坐标
    bool valid;     // 是否有效
} RectMidpointResult;

// 解析矩形中点数据帧的函数
bool ParseRectMidpoints(const uint8_t *data, uint8_t length, RectMidpointResult *result);
void GetPoint(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *x, int *y);
extern RectMidpointResult rectResults;
extern int index_point;
#endif // RECT_MIDPOINT_TRACKER_H
