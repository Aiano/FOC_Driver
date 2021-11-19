//
// Created by Aiano on 2021/11/8.
//

#include "FOC.h"

void FOC_Driver_Init(FOCDriverType *driver) {
    pwm_init(driver->phaseATIMHandle, driver->phaseAChannel);
    pwm_init(driver->phaseBTIMHandle, driver->phaseBChannel);
    pwm_init(driver->phaseCTIMHandle, driver->phaseCChannel);

    pwm_set_value(driver->phaseATIMHandle, driver->phaseAChannel, 0);
    pwm_set_value(driver->phaseBTIMHandle, driver->phaseBChannel, 0);
    pwm_set_value(driver->phaseCTIMHandle, driver->phaseCChannel, 0);
}

void FOC_Driver_Set_Target(FOCDriverType *driver, FOCMotionControlType motionControlType, float target, float Uq) {
    if (driver->modulationType == SinePWM) {
        switch (motionControlType) {
            case openLoopAngleControl:
            {
                static float last_target = 0;
                float target_error = target - last_target;
                if (target_error > 0) {
                    for (float i = last_target * driver->polar_pair_number;
                    i < target * driver->polar_pair_number; i += ANGLE_CONTROL_STEP) {
                        FOC_Open_Loop_Angle_Control(driver, i, Uq);
                        Delay_us(ANGLE_CONTROL_DELAY_US);
                    }
                } else {
                    for (float i = last_target * driver->polar_pair_number;
                    i > target * driver->polar_pair_number; i -= ANGLE_CONTROL_STEP) {
                        FOC_Open_Loop_Angle_Control(driver, i, Uq);
                        Delay_us(ANGLE_CONTROL_DELAY_US);
                    }
                }
                last_target = target;
                break;
            }
            case openLoopVelocityControl:
            {
                float delay_us = ANGLE_CONTROL_STEP/(driver->polar_pair_number * target)*1000000;
                for(float i = 0;i<_2PI;i+=ANGLE_CONTROL_STEP){
                    FOC_Open_Loop_Angle_Control(driver, i, Uq);
                    Delay_us(delay_us);
                }
                break;
            }
            default:
                break;
        }
    }
}

/**
 * @param driver
 * @param angle rad
 * @param Uq
 */
void FOC_Open_Loop_Angle_Control(FOCDriverType *driver, float angle, float Uq) {
    // Inverse Park transformation:
    float U_alpha = -Uq * sin(angle);
    float U_beta = Uq * cos(angle);

    // Inverse Clarke transformation:
    float u_a = U_alpha + driver->voltage_power_supply / 2;
    float u_b = (-U_alpha + sqrt(3) * U_beta) / 2 + driver->voltage_power_supply / 2;
    float u_c = (-U_alpha - sqrt(3) * U_beta) / 2 + driver->voltage_power_supply / 2;

    int pwm_a = (int) (u_a / driver->voltage_power_supply * driver->pwm_tim_counter_period);
    int pwm_b = (int) (u_b / driver->voltage_power_supply * driver->pwm_tim_counter_period);
    int pwm_c = (int) (u_c / driver->voltage_power_supply * driver->pwm_tim_counter_period);

    pwm_set_value(driver->phaseATIMHandle, driver->phaseAChannel, pwm_a);
    pwm_set_value(driver->phaseBTIMHandle, driver->phaseBChannel, pwm_b);
    pwm_set_value(driver->phaseCTIMHandle, driver->phaseCChannel, pwm_c);
}

/**
 * @param driver
 * @param velocity rad/s
 * @param Uq
 */
void FOC_Open_Loop_Velocity_Control(FOCDriverType *driver, float velocity, float Uq) {

}

void Delay_us(int us) {
    __HAL_TIM_SET_COUNTER(US_DELAY_TIM_HANDLE, 0);

    HAL_TIM_Base_Start(US_DELAY_TIM_HANDLE);
    while (__HAL_TIM_GET_COUNTER(US_DELAY_TIM_HANDLE) < us);
    HAL_TIM_Base_Stop(US_DELAY_TIM_HANDLE);
}


