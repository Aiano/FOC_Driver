/**
 * @file FOC.h
 * @author Aiano_czm
 * @brief
 */

#ifndef FOC_DRIVER_FOC_H
#define FOC_DRIVER_FOC_H

typedef enum{
    OPEN_LOOP_POSITION_CONTROL = 0,
    TORQUE_CONTROL,
    SPEED_CONTROL,
    POSITION_CONTROL
} FOC_CONTROL_MODE;

extern const char foc_control_mode_name[4][20];

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
#endif //FOC_DRIVER_FOC_H
