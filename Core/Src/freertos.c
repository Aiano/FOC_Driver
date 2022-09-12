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
#include "current_sense.h"

#include "gui.h"

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
static TaskHandle_t taskSelectTaskHandle = NULL;
static TaskHandle_t openLoopPosControlTaskHandle = NULL;
static TaskHandle_t openLoopVelControlTaskHandle = NULL;
static TaskHandle_t torqueControlTaskHandle = NULL;
static TaskHandle_t velocityControlTaskHandle = NULL;
static TaskHandle_t positionControlTaskHandle = NULL;
static TaskHandle_t springTaskHandle = NULL;
static TaskHandle_t knobTaskHandle = NULL;
static TaskHandle_t zeroResistanceTaskHandle = NULL;

PID_Datatype *spring_pid;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SuspendToRunOtherTask(TaskHandle_t other_task);

void TaskSelectTask(void *argument);

void OpenLoopPosControlTask(void *argument);

void OpenLoopVelControlTask(void *argument);

void TorqueControlTask(void *argument);

void VelocityControlTask(void *argument);

void PositionControlTask(void *argument);

void SpringTask(void *argument);

void DampTask(void *argument);

void KnobTask(void *argument);

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
    xTaskCreate(OpenLoopPosControlTask, "OpenPos", 64, NULL, 6, &openLoopPosControlTaskHandle);
    vTaskSuspend(openLoopPosControlTaskHandle);

    xTaskCreate(OpenLoopVelControlTask, "OpenVel", 64, NULL, 6, &openLoopVelControlTaskHandle);
    vTaskSuspend(openLoopVelControlTaskHandle);

    xTaskCreate(TorqueControlTask, "TorCtrl", 64, NULL, 6, &torqueControlTaskHandle);
    vTaskSuspend(torqueControlTaskHandle);

    xTaskCreate(VelocityControlTask, "VelCtrl", 64, NULL, 6, &velocityControlTaskHandle);
    vTaskSuspend(velocityControlTaskHandle);

    xTaskCreate(PositionControlTask, "PosCtrl", 64, NULL, 6, &positionControlTaskHandle);
    vTaskSuspend(positionControlTaskHandle);

    xTaskCreate(SpringTask, "Spring", 64, NULL, 6, &springTaskHandle);
    vTaskSuspend(springTaskHandle);

    xTaskCreate(KnobTask, "Knob", 64, NULL, 6, &knobTaskHandle);
    vTaskSuspend(knobTaskHandle);

    xTaskCreate(TaskSelectTask, "Select", 64, NULL, 6, &taskSelectTaskHandle);

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 64);
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
void SuspendToRunOtherTask(TaskHandle_t other_task) {
    if (other_task == taskSelectTaskHandle) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    }
    button_reset_all_flags();
    FOC_SVPWM(0, 0, 0);
    vTaskResume(other_task);
    vTaskSuspend(NULL);
}

void TaskSelectTask(void *argument) {
    FOC_CONTROL_MODE mode = OPEN_LOOP_POSITION_CONTROL;
    portENTER_CRITICAL();
    gui_draw_mode_selection(mode);
    portEXIT_CRITICAL();
    TickType_t xLastWakeTime = xTaskGetTickCount();

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                if (mode) mode--;
                else mode = FOC_CONTROL_MODE_NUM - 1;
                gui_draw_mode_selection(mode);

            } else if (button_right_press_pending_flag) {
                mode++;
                if (mode > FOC_CONTROL_MODE_NUM - 1) mode = 0;
                gui_draw_mode_selection(mode);

            } else if (button_confirm_press_pending_flag) {
                switch (mode) {
                    case OPEN_LOOP_POSITION_CONTROL:
                        SuspendToRunOtherTask(openLoopPosControlTaskHandle);
                        break;
                    case OPEN_LOOP_SPEED_CONTROL:
                        SuspendToRunOtherTask(openLoopVelControlTaskHandle);
                        break;
                    case TORQUE_CONTROL:
                        SuspendToRunOtherTask(torqueControlTaskHandle);
                        break;
                    case SPEED_CONTROL:
                        SuspendToRunOtherTask(velocityControlTaskHandle);
                        break;
                    case POSITION_CONTROL:;
                        SuspendToRunOtherTask(positionControlTaskHandle);
                        break;
                    case SPRING:
                        spring_pid = &pid_spring;
                        SuspendToRunOtherTask(springTaskHandle);
                        break;
                    case SPRING_WITH_DAMP:
                        spring_pid = &pid_spring_with_damp;
                        SuspendToRunOtherTask(springTaskHandle);
                        break;
                    case DAMP:
                        spring_pid = &pid_damp;
                        SuspendToRunOtherTask(springTaskHandle);
                        break;
                    case KNOB:
                        SuspendToRunOtherTask(knobTaskHandle);
                        break;
                    case ZERO_RESISTANCE:
                        spring_pid = &pid_zero_resistance;
                        SuspendToRunOtherTask(springTaskHandle);
                        break;
                }
                gui_draw_mode_selection(mode);
                continue;
            }

            button_reset_all_flags();
        }


        vTaskDelayUntil(&xLastWakeTime,
                        10);
    }
}

void OpenLoopPosControlTask(void *argument) {
    float angle = 0, Ud = 1;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                angle -= _PI_2;
            } else if (button_right_press_pending_flag) {
                angle += _PI_2;
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                continue;
            }
            button_reset_all_flags();
        }

        FOC_SVPWM(0, Ud, angle);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void OpenLoopVelControlTask(void *argument) {
    float angle = 0, Ud = 1, delta_angle = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                delta_angle -= 0.01;
                if (delta_angle < -_PI) delta_angle = -_PI;
            } else if (button_right_press_pending_flag) {
                delta_angle += 0.01;
                if (delta_angle > _PI) delta_angle = _PI;
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                continue;
            }
            button_reset_all_flags();
        }
        angle += delta_angle;
        FOC_SVPWM(0, Ud, angle);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void TorqueControlTask(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float Uq = 0;

    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                Uq -= 1.0;
            } else if (button_right_press_pending_flag) {
                Uq += 1.0;
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                continue;
            }
            button_reset_all_flags();
        }
        FOC_open_loop_voltage_control_loop(Uq);

        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void VelocityControlTask(void *argument) {
    float velocity = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                velocity -= 5;
            } else if (button_right_press_pending_flag) {
                velocity += 5;
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                continue;
            }
            button_reset_all_flags();
        }


        FOC_velocity_control_loop(velocity);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void PositionControlTask(void *argument) {
    float angle = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    gui_draw_position_mode(angle, 1);

    while (1) {
        if (button_press_pending_flag) {
            if (button_left_press_pending_flag) {
                angle -= _PI_2;
            } else if (button_right_press_pending_flag) {
                angle += _PI_2;
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                gui_draw_position_mode(angle, 1);
                continue;
            }
            button_reset_all_flags();
            gui_draw_position_mode(angle, 0);
        }

        FOC_position_control_loop(angle);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void SpringTask(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        if (button_press_pending_flag) {
            if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                continue;
            }
            button_reset_all_flags();
        }
        FOC_spring_loop(0, spring_pid);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

void DampTask(void *argument) {

}

void KnobTask(void *argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Settings
    uint8_t select_param = 0;
    const uint8_t max_select_param = 3;
    uint8_t change_param = 0;

    uint8_t values[3] = {4, 5, 4}; // [0]: sector_num [1]: k [2]: max_force

    pid_knob.Kp = values[1];
    pid_knob.max_u = values[2];
    pid_knob.min_u = -values[2];
    gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 1);

    while (1) {
        if (button_press_pending_flag) {
            if (button_confirm_press_pending_flag) {
                if(select_param){
                    change_param = !change_param;
                }
            } else if (button_left_press_pending_flag) {
                if(change_param){
                    values[select_param - 1] --;

                    // reconfigure
                    pid_knob.Kp = values[1];
                    pid_knob.max_u = values[2];
                    pid_knob.min_u = -values[2];
                    gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
                }else{
                    if (select_param == 0) {
                        select_param = max_select_param;
                    } else select_param--;
                }
            } else if (button_right_press_pending_flag) {
                if(change_param){
                    values[select_param - 1] ++;

                    // reconfigure
                    pid_knob.Kp = values[1];
                    pid_knob.max_u = values[2];
                    pid_knob.min_u = -values[2];
                    gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
                }else{
                    if (select_param == max_select_param) {
                        select_param = 0;
                    } else select_param++;
                }
            } else if (button_cancel_press_pending_flag) {
                SuspendToRunOtherTask(taskSelectTaskHandle);
                gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 1);
                continue;
            }
            button_reset_all_flags();
            gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
        }

        FOC_knob_loop(values[0]);
        vTaskDelayUntil(&xLastWakeTime,
                        1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
    }
}

/* USER CODE END Application */

