/**
 * @file pwm.cpp
 * @brief pwm controller
 * @author Aiano
 */
#include "pwm.h"
#include "main.h"


void pwm_init(TIM_HandleTypeDef *tim_handle, uint32_t Channel) {
    HAL_TIM_PWM_Start(tim_handle, Channel);
}

void pwm_set_value(TIM_HandleTypeDef *tim_handle, uint32_t Channel, int value) {
    if (value > 200) value = 200;
    if (value < 0) value = 0;
    __HAL_TIM_SET_COMPARE(tim_handle, Channel, value);
}

void pwm_de_init(TIM_HandleTypeDef *tim_handle, uint32_t Channel) {
    HAL_TIM_PWM_Stop(tim_handle, Channel);
}


