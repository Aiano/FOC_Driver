/**
 * @file FOC.h
 * @author Aiano_czm
 * @brief
 */

#ifndef FOC_DRIVER_FOC_H
#define FOC_DRIVER_FOC_H

#include "FOC_PID.h"

typedef enum{
    OPEN_LOOP_POSITION_CONTROL = 0,
    OPEN_LOOP_SPEED_CONTROL,
    TORQUE_CONTROL,
    SPEED_CONTROL,
    POSITION_CONTROL,
    SPRING,
    SPRING_WITH_DAMP,
    DAMP,
    KNOB,
    ZERO_RESISTANCE
} FOC_CONTROL_MODE;

#define FOC_CONTROL_MODE_NUM 10

void FOC_init();
void FOC_electrical_angle_calibration();
void FOC_SVPWM(float Uq, float Ud, float angle);
float FOC_get_mechanical_angle();
float FOC_electrical_angle();
float FOC_get_velocity();

// Some modes to choose
void FOC_open_loop_voltage_control_loop(float Uq);

void FOC_velocity_control_loop(float target_velocity);
void FOC_position_control_loop(float target_angle);
void FOC_spring_loop(float target_angle, PID_Datatype *pid);
void FOC_knob_loop(uint8_t sector_num);
#endif //FOC_DRIVER_FOC_H
