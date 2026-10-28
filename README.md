# 视觉跟随云台 + 循迹小车（STM32F103ZET6）

基于 STM32F103ZET6 的小车控制程序：**摄像头识别目标 → 串口回传坐标 → 双步进电机驱动云台跟随**，同时保留 8 路灰度循迹与轮速闭环。

## 硬件组成

| 部分 | 器件 / 说明 |
|---|---|
| 主控 | STM32F103ZET6，CubeMX + HAL 库 |
| 视觉 | 摄像头 / 视觉模块，串口输出目标矩形中点坐标 |
| 云台 | 双步进电机 + Emm_V5 闭环驱动器（上下两轴） |
| 循迹 | 8 路灰度传感器（No_Mcu_Ganv 方案） |
| 车轮 | 直流电机 + 编码器测速 |
| 姿态 | IMU（串口） |
| 人机 | OLED 显示、按键 |

## 软件结构

| 模块 | 主要文件 | 作用 |
|---|---|---|
| 主流程 | `main.c`、`question.c` | 20 ms 控制节拍；`question_1` 启动循迹、`question_2` 启动云台跟随 |
| 视觉解析 | `rect_midpoint_tracker.c` | 串口帧协议（0x55/0x56 或 0xAA/0xBB 帧头）解析目标矩形中点，输出坐标 |
| 云台控制 | `stepper_motor.c`、`Emm_V5.c` | 目标坐标 → 双轴偏差 → 步进位置闭环 |
| 循迹 | `Line.c`、`No_Mcu_Ganv_Grayscale_Sensor.c` | 灰度质心求线位置偏差 + PD 转向 |
| 速度环 | `PID.c`、`Motor.c`、`Encode.c` | 左右轮速度 PID，TIM4 双通道 PWM |
| 姿态 / 人机 | `imu.c`、`oled.c`、`key.c`、`hardware_iic.c` | 偏航角、显示与交互、软 I2C |

控制节拍集中在 `main.c` 的定时器回调：TIM 每 20 ms 置标志位，主循环统一执行一次控制决策；中断只负责采数与发脉冲。

## 编译

- Keil MDK：打开 `MDK-ARM/CAR.uvprojx`
- CubeMX 工程：`car.ioc`

> 仓库不含编译产物（`.gitignore` 已排除 `MDK-ARM/CAR/`），克隆后需自行编译。

## 演示

| 整机运行 | 视觉跟踪界面（TRACK） |
|---|---|
| ![整机](docs/frames/demo-1-2.jpg) | ![视觉跟踪](docs/frames/demo-2-2.jpg) |

更多画面见 `docs/frames/`，完整视频：`docs/video/demo-1.mp4`、`docs/video/demo-2.mp4`。