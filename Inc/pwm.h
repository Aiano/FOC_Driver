/**
 * @file pwm.h
 * @brief pwm controller header
 * @author Aiano
 */
#ifndef FOC_DRIVER_PWM_H
#define FOC_DRIVER_PWM_H

#include "tim.h"

/**
 * @brief init pwm
 *
 */
void pwm_init(TIM_HandleTypeDef *tim_handle, uint32_t Channel);

void pwm_set_value(TIM_HandleTypeDef *tim_handle, uint32_t Channel, int value);

void pwm_de_init(TIM_HandleTypeDef *tim_handle, uint32_t Channel);

#endif //FOC_DRIVER_PWM_H
