#include "Emm_V5.h"
#include "usart.h"

// 宏定义串口句柄
#define EMM_UART_HANDLE &huart3

/**********************************************************
***	Emm_V5.0步进闭环控制例程（HAL库适配版）
***	修改说明：使用HAL_UART_Transmit_DMA替代原串口发送函数
**********************************************************/

/**
  * @brief    将当前位置清零
  * @param    addr  ：电机地址
  */
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x0A;         // 功能码
  cmd[2] = 0x6D;         // 辅助码
  cmd[3] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 4 );
}

/**
  * @brief    解除堵转保护
  * @param    addr  ：电机地址
  */
void Emm_V5_Reset_Clog_Pro(uint8_t addr)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x0E;         // 功能码
  cmd[2] = 0x52;         // 辅助码
  cmd[3] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 4 );
}

/**
  * @brief    读取系统参数
  * @param    addr  ：电机地址
  * @param    s     ：系统参数类型
  */
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
  uint8_t i = 0;
  uint8_t cmd[16] = {0};
  
  cmd[i++] = addr;       // 地址

  switch(s)
  {
    case S_VER  : cmd[i++] = 0x1F; break;
    case S_RL   : cmd[i++] = 0x20; break;
    case S_PID  : cmd[i++] = 0x21; break;
    case S_VBUS : cmd[i++] = 0x24; break;
    case S_CPHA : cmd[i++] = 0x27; break;
    case S_ENCL : cmd[i++] = 0x31; break;
    case S_TPOS : cmd[i++] = 0x33; break;
    case S_VEL  : cmd[i++] = 0x35; break;
    case S_CPOS : cmd[i++] = 0x36; break;
    case S_PERR : cmd[i++] = 0x37; break;
    case S_FLAG : cmd[i++] = 0x3A; break;
    case S_ORG  : cmd[i++] = 0x3B; break;
    case S_Conf : cmd[i++] = 0x42; cmd[i++] = 0x6C; break;
    case S_State: cmd[i++] = 0x43; cmd[i++] = 0x7A; break;
    default: break;
  }

  cmd[i++] = 0x6B;       // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, i );
}

/**
  * @brief    修改开环/闭环控制模式
  * @param    addr     ：电机地址
  * @param    svF      ：是否存储标志
  * @param    ctrl_mode：控制模式（0-3）
  */
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, uint8_t ctrl_mode)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x46;         // 功能码
  cmd[2] = 0x69;         // 辅助码
  cmd[3] = (uint8_t)svF; // 存储标志
  cmd[4] = ctrl_mode;    // 控制模式
  cmd[5] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 6 );
}

/**
  * @brief    使能信号控制
  * @param    addr  ：电机地址
  * @param    state ：使能状态
  * @param    snF   ：多机同步标志
  */
void Emm_V5_En_Control(uint8_t addr, bool state, bool snF)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0xF3;         // 功能码
  cmd[2] = 0xAB;         // 辅助码
  cmd[3] = (uint8_t)state; // 使能状态
  cmd[4] = (uint8_t)snF; // 同步标志
  cmd[5] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 6 );
}

/**
  * @brief    速度模式
  * @param    addr：电机地址
  * @param    dir ：方向（0=CW，其他=CCW）
  * @param    vel ：速度（0-5000RPM）
  * @param    acc ：加速度（0-255）
  * @param    snF ：多机同步标志
  */
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF)
{
  uint8_t cmd[16] = {0};

  cmd[0] = addr;                  // 地址
  cmd[1] = 0xF6;                  // 功能码
  cmd[2] = dir;                   // 方向
  cmd[3] = (uint8_t)(vel >> 8);   // 速度高8位
  cmd[4] = (uint8_t)(vel & 0xFF); // 速度低8位
  cmd[5] = acc;                   // 加速度
  cmd[6] = (uint8_t)snF;          // 同步标志
  cmd[7] = 0x6B;                  // 校验字节
  
  HAL_UART_Transmit_IT(EMM_UART_HANDLE, cmd, 8 );
}

/**
  * @brief    位置模式
  * @param    addr：电机地址
  * @param    dir ：方向（0=CW，其他=CCW）
  * @param    vel ：速度（0-5000RPM）
  * @param    acc ：加速度（0-255）
  * @param    clk ：脉冲数（0-2^32-1）
  * @param    raF ：相对/绝对标志
  * @param    snF ：多机同步标志
  */
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF)
{
  uint8_t cmd[13] = {0};

  cmd[0]  = addr;                   // 地址
  cmd[1]  = 0xFD;                   // 功能码
  cmd[2]  = dir;                    // 方向
  cmd[3]  = (uint8_t)(vel >> 8);    // 速度高8位
  cmd[4]  = (uint8_t)(vel & 0xFF);  // 速度低8位
  cmd[5]  = acc;                    // 加速度
  cmd[6]  = (uint8_t)(clk >> 24);   // 脉冲数高8位
  cmd[7]  = (uint8_t)(clk >> 16);
  cmd[8]  = (uint8_t)(clk >> 8);
  cmd[9]  = (uint8_t)(clk & 0xFF);  // 脉冲数低8位
  cmd[10] = (uint8_t)raF;           // 相对/绝对标志
  cmd[11] = (uint8_t)snF;           // 同步标志
  cmd[12] = 0x6B;                   // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd ,13);
}

/**
  * @brief    立即停止（所有模式通用）
  * @param    addr  ：电机地址
  * @param    snF   ：多机同步标志
  */
void Emm_V5_Stop_Now(uint8_t addr, bool snF)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0xFE;         // 功能码
  cmd[2] = 0x98;         // 辅助码
  cmd[3] = (uint8_t)snF; // 同步标志
  cmd[4] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 5 );
}

/**
  * @brief    多机同步运动
  * @param    addr  ：电机地址
  */
void Emm_V5_Synchronous_motion(uint8_t addr)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0xFF;         // 功能码
  cmd[2] = 0x66;         // 辅助码
  cmd[3] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 4 );
}

/**
  * @brief    设置单圈回零的零点位置
  * @param    addr  ：电机地址
  * @param    svF   ：是否存储标志
  */
void Emm_V5_Origin_Set_O(uint8_t addr, bool svF)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x93;         // 功能码
  cmd[2] = 0x88;         // 辅助码
  cmd[3] = (uint8_t)svF; // 存储标志
  cmd[4] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 5 );
}

/**
  * @brief    修改回零参数
  * @param    （参数说明见原函数）
  */
void Emm_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF)
{
  uint8_t cmd[32] = {0};
  uint8_t i = 0;
  
  cmd[i++] = addr;
  cmd[i++] = 0x4C;
  cmd[i++] = 0xAE;
  cmd[i++] = (uint8_t)svF;
  cmd[i++] = o_mode;
  cmd[i++] = o_dir;
  cmd[i++] = (uint8_t)(o_vel >> 8);
  cmd[i++] = (uint8_t)(o_vel & 0xFF);
  cmd[i++] = (uint8_t)(o_tm >> 24);
  cmd[i++] = (uint8_t)(o_tm >> 16);
  cmd[i++] = (uint8_t)(o_tm >> 8);
  cmd[i++] = (uint8_t)(o_tm & 0xFF);
  cmd[i++] = (uint8_t)(sl_vel >> 8);
  cmd[i++] = (uint8_t)(sl_vel & 0xFF);
  cmd[i++] = (uint8_t)(sl_ma >> 8);
  cmd[i++] = (uint8_t)(sl_ma & 0xFF);
  cmd[i++] = (uint8_t)(sl_ms >> 8);
  cmd[i++] = (uint8_t)(sl_ms & 0xFF);
  cmd[i++] = (uint8_t)potF;
  cmd[i++] = 0x6B;
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, i );
}

/**
  * @brief    触发回零
  * @param    addr   ：电机地址
  * @param    o_mode ：回零模式（0-3）
  * @param    snF   ：多机同步标志
  */
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x9A;         // 功能码
  cmd[2] = o_mode;       // 回零模式
  cmd[3] = (uint8_t)snF; // 同步标志
  cmd[4] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 5 );
}

/**
  * @brief    强制中断并退出回零
  * @param    addr  ：电机地址
  */
void Emm_V5_Origin_Interrupt(uint8_t addr)
{
  uint8_t cmd[16] = {0};
  
  cmd[0] = addr;         // 地址
  cmd[1] = 0x9C;         // 功能码
  cmd[2] = 0x48;         // 辅助码
  cmd[3] = 0x6B;         // 校验字节
  
  HAL_UART_Transmit_DMA(EMM_UART_HANDLE, cmd, 4 );
}
PID_t Motor_X=
{
	.Kp=0.2,
	.Ki=0.,
	.Kd=0.1,
	.OutMax=20,
	.OutMin=-20,
	.Out=0

};

PID_t Motor_Y=
{
	.Kp=0.2,
	.Ki=0,
	.Kd=0.1,
	.OutMax=15,
	.OutMin=-15,
	.Out=0

};

