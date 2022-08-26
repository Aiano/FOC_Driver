/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define CS1_Pin GPIO_PIN_0
#define CS1_GPIO_Port GPIOA
#define CS2_Pin GPIO_PIN_1
#define CS2_GPIO_Port GPIOA
#define CS3_Pin GPIO_PIN_2
#define CS3_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_10
#define KEY1_GPIO_Port GPIOB
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define KEY2_Pin GPIO_PIN_11
#define KEY2_GPIO_Port GPIOB
#define KEY2_EXTI_IRQn EXTI15_10_IRQn
#define KEY3_Pin GPIO_PIN_12
#define KEY3_GPIO_Port GPIOB
#define KEY3_EXTI_IRQn EXTI15_10_IRQn
#define KEY4_Pin GPIO_PIN_13
#define KEY4_GPIO_Port GPIOB
#define KEY4_EXTI_IRQn EXTI15_10_IRQn
#define EN_Pin GPIO_PIN_15
#define EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
