/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FOC.h"
#include "FOC_utils.h"
//#include "AS5600.h"
//#include "current_sense.h"

//#include "st7735.h"
//#include "fonts.h"
//#include "image.h"

#include "bsp_button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static TaskHandle_t velocityControlTaskHandle = NULL;
static TaskHandle_t positionControlTaskHandle = NULL;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void VelocityControlTask(void *argument);
void PositionControlTask(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

    xTaskCreate(VelocityControlTask, "VelCtrl", 512, NULL, 6, velocityControlTaskHandle);
    vTaskSuspend(velocityControlTaskHandle);

    xTaskCreate(PositionControlTask, "PosCtrl", 512, NULL, 6, velocityControlTaskHandle);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void VelocityControlTask(void *argument) {
    float velocity = 0;
    while (1) {
        if (button_left_press_pending_flag) {
            velocity -= 5;
            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        } else if (button_right_press_pending_flag) {
            velocity += 5;
            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        }
        button_reset_all_flags();

        portENTER_CRITICAL();
        FOC_velocity_control_loop(velocity);
        portEXIT_CRITICAL();
    }
}

void PositionControlTask(void *argument){
    float angle = 0;
    while (1) {
        if (button_left_press_pending_flag) {
            angle -= _PI_2;
            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        } else if (button_right_press_pending_flag) {
            angle += _PI_2;
            HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
        }
        button_reset_all_flags();

        portENTER_CRITICAL();
        FOC_position_control_loop(angle);
        portEXIT_CRITICAL();
    }
}
/* USER CODE END Application */

