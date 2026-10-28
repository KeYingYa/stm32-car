/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Direction_Pin GPIO_PIN_6
#define Direction_GPIO_Port GPIOC
#define YunTableUp_Pin GPIO_PIN_7
#define YunTableUp_GPIO_Port GPIOC
#define YunTableDown_Pin GPIO_PIN_8
#define YunTableDown_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOG
#define MotorB1_Pin GPIO_PIN_15
#define MotorB1_GPIO_Port GPIOG
#define MotorB2_Pin GPIO_PIN_3
#define MotorB2_GPIO_Port GPIOB
#define PWMB_Pin GPIO_PIN_5
#define PWMB_GPIO_Port GPIOB
#define Stby_Pin GPIO_PIN_9
#define Stby_GPIO_Port GPIOB
#define MotorA2_Pin GPIO_PIN_0
#define MotorA2_GPIO_Port GPIOE
#define MotorA1_Pin GPIO_PIN_1
#define MotorA1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
