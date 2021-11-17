//
// Created by Aiano on 2021/11/8.
//

#ifndef FOC_DRIVER_FOC_H
#define FOC_DRIVER_FOC_H

#include <cmath>
#include "pwm.h"
#include "tim.h"

#define _PI 3.14
#define _2PI 6.28
#define ANGLE_CONTROL_STEP 0.005
#define ANGLE_CONTROL_DELAY_US 250
#define US_DELAY_TIM_HANDLE &htim2

enum FOCModulationType {
    SinePWM = 0,
    SpaceVectorPWM
};

enum FOCMotionControlType {
    angleControl = 0,
    velocityControl
};

typedef struct {
    FOCModulationType modulationType;
    float voltage_power_supply;
    int pwm_tim_counter_period;
    int polar_pair_number;
    TIM_HandleTypeDef *phaseATIMHandle;
    uint32_t phaseAChannel;
    TIM_HandleTypeDef *phaseBTIMHandle;
    uint32_t phaseBChannel;
    TIM_HandleTypeDef *phaseCTIMHandle;
    uint32_t phaseCChannel;
} FOCDriverType;

void FOC_Driver_Init(FOCDriverType *driver);

void FOC_Driver_Set_Target(FOCDriverType *driver, FOCMotionControlType motionControlType, float target, float Uq);

void FOC_Angle_Control(FOCDriverType *driver, float angle, float Uq);

void FOC_Velocity_Control(FOCDriverType *driver, float velocity, float Uq);

void Delay_us(int us);

#endif //FOC_DRIVER_FOC_H
