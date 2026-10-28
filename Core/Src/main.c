/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Motor.h"
#include "Encode.h"
#include "stdlib.h"
#include "stdio.h"
#include "imu.h"
#include "string.h"
#include "PID.h"
#include "Line.h"
#include "oled.h"
#include "Emm_V5.h"
#include "rect_midpoint_tracker.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "stepper_motor.h"
#include "question.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// 传感器数据结构

int stop_flag=0;
int question2_flag=0;
int question3_flag=0;
int question4_flag=0;
unsigned char rx_buff[256] = {0};
#define UART_BUFFER_SIZE 10
uint8_t uartBuffer[UART_BUFFER_SIZE];
int x;
int y;
int flag=0;
unsigned short Anolog[8]={0};
unsigned short white[8]={3197, 3109, 3146, 3175, 3188, 3152, 3061, 3116};
unsigned short black[8]={2231, 1474, 1284, 2044, 1720, 1074, 698, 1470};
unsigned short Normal[8];
unsigned char Digtal;
uint8_t successResponse[] = {0x01, 0xF6, 0x02, 0x6B};
uint8_t uart3RxBuffer[4];  // 用于接收UART3的返回数据
uint8_t txReady = 1;       // 发送就绪标志，1表示可以发送
int speed=10;
Line line;
int N=1;
No_MCU_Sensor sensor;
uint8_t PIDNum=0;
uint8_t ShowNum = 0;  // OLED显示模式
StepperMotor stepper_down;
StepperMotor stepper_up;


// 按键标志
uint8_t key_flag1 = 0;
uint8_t key_flag2 = 0;
uint8_t key_flag3 = 0;
uint8_t key_flag4 = 0;
uint8_t key_flag5 = 0;

// 串口接收缓冲区
#define RX_BUFFER_SIZE 128
uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  GPIO外部中断回调函数
  * @param  GPIO_Pin 触发中断的引脚
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	  static uint32_t last_key1_time = 0;
    static uint32_t last_key2_time = 0;
    static uint32_t last_key3_time = 0;
    static uint32_t last_key4_time = 0;
    static uint32_t last_key5_time = 0;
    const uint32_t DEBOUNCE_TIME = 200;  // 消抖时间(ms)
    
    // 按键1处理 (GPIO_PIN_3)
    if (GPIO_Pin == GPIO_PIN_3 && key_flag3 == 0)
    {    
        // 按键消抖检查
        uint32_t now = HAL_GetTick();
        if (now - last_key3_time < DEBOUNCE_TIME) return;
        last_key3_time = now;
        
        key_flag3 = 1;


        key_flag3 = 0;
    }
    // 按键2处理 (GPIO_PIN_4)
    else if (GPIO_Pin == GPIO_PIN_4 && key_flag4 == 0)
    {
        // 按键消抖检查
        uint32_t now = HAL_GetTick();
        if (now - last_key4_time < DEBOUNCE_TIME) return;
        last_key4_time = now;
        
        key_flag4 = 1;

        key_flag4 = 0;
    }
else if (GPIO_Pin == GPIO_PIN_1 && key_flag1 == 0)
{
    uint32_t now = HAL_GetTick();
    if (now - last_key1_time < DEBOUNCE_TIME) return;
    
    last_key1_time = now;
//    PIDNum = (PIDNum < 2) ? PIDNum + 1 : 0; // 优化循环逻辑
				ShowNum = (ShowNum < 6) ? ShowNum + 1 : 0;
			
    key_flag1 = 0;
}// 按键1
else if (GPIO_Pin == GPIO_PIN_2 && key_flag2 == 0)
{
    uint32_t now = HAL_GetTick();
    if (now - last_key2_time < DEBOUNCE_TIME) return;
    // 按键2
    last_key2_time = now;
    
if(ShowNum==1)
{
 question2_flag=1;

}
else if (ShowNum==2)
{
 question3_flag=1;

}
else if (ShowNum==3)
{

	 question4_flag=1;
}	else if (ShowNum==0)
{
	Line_Start_flag=1;
}
//		    if (PIDNum == 0) {
//        Motor_X.Kp += 0.05; // 对应PIDNum为1时，pidValue自增0.1
//    } else if (PIDNum == 1) {
//        Motor_X.Ki += 0.01; // 对应PIDNum为2时，pidValue自增0.1
//    } else if (PIDNum == 2) {
//        Motor_X.Kd += 0.05; // 对应PIDNum为3时，pidValue自增0.1
//    }

    key_flag2 = 0;
}
	else if (GPIO_Pin == GPIO_PIN_5 && key_flag5 == 0)
	{
			// 按键3
			uint32_t now = HAL_GetTick();
			if (now - last_key5_time < DEBOUNCE_TIME) return;
			last_key5_time = now;
			
			key_flag5 = 1;
			
   N = (N + 1) % 5; // 自增并取模
    if (N == 0) {
        N = 5; // 如果N为0（即5%5的结果），则将其设置为5
    }
			key_flag5 = 0;
	}
}

/**
  * @brief  TIM周期中断回调函数
  * @param  htim TIM句柄
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 10ms中断处理
    if (htim == &htim7) {
		        // 1. 计算轮子速度
      WheelSpeed_Calculate(&wheel1);
      WheelSpeed_Calculate(&wheel2);
        // 2. 更新PID控制器
      PID_Wheel1.Actual = fabs(wheel1.wheel_speed_rpm);
      PID_Wheel2.Actual = fabs(wheel2.wheel_speed_rpm);    
      PID_Update(&PID_Wheel1);
      PID_Update(&PID_Wheel2);
        // 3. 设置PWM输出
      __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, (uint32_t)PID_Wheel2.Out);
      __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, (uint32_t)PID_Wheel1.Out);

			Get_Normalize_For_User(&sensor,Normal);
			LineProgress(Normal, &line);
			PID_Line.Target=0;
			PID_Line.Actual=line.error;
			PID_Update(&PID_Line);
			if(line.position==-999)
			{
				SetMotor(0,0);
			}
			else{
				if(Line_Start_flag==1)
				{
			SetMotor(80,-PID_Line.Out);
				
				}
				else
				{
				SetMotor(0,0);
				}
			}

		if(rectResults.valid && YunTai_flag==1){
	    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);		
if (Motor_X.Out > 0) {
    // 正值输出时设置为顺时针方向
    Stepper_SetDirection(&stepper_down, STEPPER_DIR_CW);
    // 使用绝对值作为速度（确保速度为正数）
    Stepper_SetSpeed(&stepper_down, Motor_X.Out);
} else if (Motor_X.Out <= 0) {
    // 负值输出时设置为逆时针方向
    Stepper_SetDirection(&stepper_down, STEPPER_DIR_CCW);
    // 使用绝对值作为速度（确保速度为正数）
    Stepper_SetSpeed(&stepper_down, -Motor_X.Out);
} 

// 同样的方式处理Y轴
if (Motor_Y.Out > 0) {
    Stepper_SetDirection(&stepper_up, STEPPER_DIR_CW);
    Stepper_SetSpeed(&stepper_up, Motor_Y.Out);
} else if (Motor_Y.Out <=0) {
    Stepper_SetDirection(&stepper_up, STEPPER_DIR_CCW);
    Stepper_SetSpeed(&stepper_up, -Motor_Y.Out);
} 

}



else if (stop_flag==1)
{	   
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);	
}

else if (stop_flag==0 && question3_flag==0)
{	   
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);	
}


    }
    // 50ms中断处理
    else if (htim == &htim6) 
		{
			        
//		//云台控制
		Motor_X.Target=320;
	Motor_Y.Target=240;
	// 更新实际位置
Motor_X.Actual = rectResults.x;
Motor_Y.Actual = rectResults.y;

// 执行PID计算
PID_Update(&Motor_X);
PID_Update(&Motor_Y);

// 根据PID输出自动设置步进电机方向和速度
			
//			

    }
}

/**
  * @brief  UART接收完成回调函数
  * @param  huart UART句柄
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 矩形中心
    if (huart == &huart4) 
    {
flag++;
        HAL_UART_Receive_DMA(&huart4, uartBuffer, sizeof(uartBuffer));
        ParseRectMidpoints(uartBuffer, sizeof(uartBuffer), &rectResults);
    }
}

void EnterDeadLock(void) {
    // 1. 关闭所有中断（包括不可屏蔽中断以外的所有中断）
    __disable_irq();
    
    // 2. 关闭 systick 定时器（避免内核滴答中断）
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    
    // 3. 进入无限空循环，且不执行任何可能触发中断的操作
    while (1) {
        // 空循环，CPU持续在此处运行，不响应任何中断
        // 禁止使用任何可能触发中断的函数（如HAL_Delay依赖systick）
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    // 初始化车轮结构
	
   Wheel_Init(&wheel1, &htim1, 28.0f, 52.0f);
   Wheel_Init(&wheel2, &htim2, 28.0f, 52.0f);
	
	    Stepper_Init(&stepper_down, &htim3, TIM_CHANNEL_1,
                GPIOF, GPIO_PIN_2,  // 方向引脚PA0
                3200, 72000000);    // 3200步/圈，72MHz时钟
		
	    Stepper_Init(&stepper_up, &htim3, TIM_CHANNEL_2,
                GPIOF, GPIO_PIN_3,  // 方向引脚PA0
                3200, 72000000);    // 3200步/圈，72MHz时钟
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM8_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  MX_ADC2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    HAL_Delay(20);
    OLED_Init();
		//视觉接受
		
    HAL_UART_Receive_DMA(&huart4, uartBuffer,sizeof(uartBuffer));
    // 启动PWM和编码器
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

		
    HAL_TIM_Base_Start_IT(&htim1); 
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);
    // yunt
	
    // 新增静态变量记录电机是否否已启动过
static uint8_t motor_started = 0;



			HAL_Delay(100);

	No_MCU_Ganv_Sensor_Init_Frist(&sensor);
	No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
	Get_Anolog_Value(&sensor,Anolog);
		No_MCU_Ganv_Sensor_Init(&sensor,white,black);
			LineInit(&line, 1.2f);	




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{
		No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
// 替换PID部分代码，测试电机是否动作


        // OLED显示更新
        OLED_NewFrame();
        if (ShowNum == 0) {
            // 显示PID参数
            OLED_Printf(1, "question1");    
					OLED_Printf(2, "val:%d",rectResults.valid); 						
 					OLED_Printf(3, "%.2f-%.2f-%.2f",Motor_X.Kp,Motor_X.Ki,Motor_X.Kd); 	
					OLED_Printf(4, "flag:%d",flag); 	
									
        } 
				else if (ShowNum == 1) 
				{
         
            OLED_Printf(1, "question2");      
        } 
				else if (ShowNum == 2)
				{
            OLED_Printf(1, "question3");  
					OLED_Printf(2, "valid:%d",rectResults.valid); 					
				}
				else if (ShowNum == 3)
				{
            OLED_Printf(1, "question4");   
					
				}			
				else if (ShowNum == 4)
				{
            OLED_Printf(1, "question5");   
					
				}			
				else if (ShowNum == 5)
				{
            OLED_Printf(1, "question6");   
					
				}	
				else if (ShowNum == 6)
				{
					OLED_Printf(1, "N:%d",N);   
					
				}					
				
		
				
        OLED_ShowFrame();
				if (N == 1)
				{
				// N=1时，阈值 = 22
				if (wheel1.wheel_revolutions > 22)
				{
						Line_Start_flag = 0;
				}
				}
				else if (N == 2)
				{
				// N=2时，阈值 = 22 + 22 = 44
				if (wheel1.wheel_revolutions > 44)
				{
						Line_Start_flag = 0;
				}
				}
				else if (N == 3)
				{
				// N=3时，阈值 = 22 + 22*2 = 66
				if (wheel1.wheel_revolutions > 66)
				{
						Line_Start_flag = 0;
				}
				}
				else if (N == 4)
				{
				// N=4时，阈值 = 22 + 22*3 = 88
				if (wheel1.wheel_revolutions > 88)
				{
						Line_Start_flag = 0;
				}
				}
				else if (N == 5)
				{
				// N=5时，阈值 = 22 + 22*4 = 110
				if (wheel1.wheel_revolutions > 110)
				{
						Line_Start_flag = 0;
				}
				}
				
				if(question2_flag==1)
				{
				YunTai_flag=1;
					HAL_Delay(1000);
					        HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
								      Stepper_SetSpeed(&stepper_down, 0);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); 
						      Stepper_SetSpeed(&stepper_up, 0);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
					EnterDeadLock();
				
				}
				// 新增静态变量记录电机是否否已启动过




if (question3_flag == 1)
{
    // 仅在电机未启动过时设置速度
    if (!motor_started)
    {
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
        Stepper_SetDirection(&stepper_down, STEPPER_DIR_CW);
        Stepper_SetSpeed(&stepper_down, 10);  // 只执行一次
        motor_started = 1;  // 标记为已启动
    }
    
    // 检测到有效信号时停止
    if (rectResults.valid)
    {
			stop_flag=1;
        Stepper_SetSpeed(&stepper_down, 0);  // 停止电机
        YunTai_flag = 1;                     // 设置标志位
        HAL_Delay(1500);
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
			      Stepper_SetSpeed(&stepper_down, 0);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); 
						      Stepper_SetSpeed(&stepper_up, 0);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
			
		EnterDeadLock();
        // 可选：重置状态（如果需要再次触发）
        // question3_flag = 0;
        // motor_started = 0;
    }
}
if(question4_flag==1)
{
Line_Start_flag=1;
	
YunTai_flag=1;
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);

}

				
				
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
