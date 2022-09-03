/**
 * @file FOC.c
 * @author Aiano_czm
 */

#include <math.h>
#include <stdio.h>
#include "FOC.h"
#include "FOC_conf.h"
#include "FOC_utils.h"
#include "FOC_PID.h"
#include "FOC_LPF.h"
#include "hardware_api.h"
#include "AS5600.h"
#include "current_sense.h"
#include "usart.h"


float zero_mechanical_angle = 0.36;
float zero_electrical_angle = 0.891460538;

float last_mechanical_angle = 0;
uint32_t pre_tick;


void FOC_init() {
    AS5600_init();
    _init3PWM();
    pid_set_parameters();
    FOC_lpf_set_parameters();
    pre_tick = HAL_GetTick();
}

void FOC_electrical_angle_calibration() {
    // FOC calibration
    FOC_SVPWM(0, 8, 0);
    HAL_Delay(400);
    zero_electrical_angle = FOC_electrical_angle();
    HAL_Delay(100);
    FOC_SVPWM(0, 0, 0);
}

void FOC_SVPWM(float Uq, float Ud, float angle) {

    int sector;

    // Nice video explaining the SpaceVectorModulation (FOC_SVPWM) algorithm
    // https://www.youtube.com/watch?v=QMSWUMEAejg

    // the algorithm goes
    // 1) Ualpha, Ubeta
    // 2) Uout = sqrt(Ualpha^2 + Ubeta^2)
    // 3) angle_el = atan2(Ubeta, Ualpha)
    //
    // equivalent to 2)  because the magnitude does not change is:
    // Uout = sqrt(Ud^2 + Uq^2)
    // equivalent to 3) is
    // angle_el = angle_el + atan2(Uq,Ud)

    float Uout = _sqrt(Ud * Ud + Uq * Uq) / VOLTAGE_LIMIT; // Actually, Uout is a ratio
    angle = _normalizeAngle(angle + atan2(Uq, Ud));

    // find the sector we are in currently
    sector = floor(angle / _PI_3) + 1;
    // calculate the duty cycles
    float T1 = _SQRT3 * _sin(sector * _PI_3 - angle) * Uout;
    float T2 = _SQRT3 * _sin(angle - (sector - 1.0f) * _PI_3) * Uout;
    // two versions possible
//    float T0 = 0; // pulled to 0 - better for low power supply voltage
    float T0 = 1 - T1 - T2; // modulation_centered around driver->voltage_limit/2


    // calculate the duty cycles(times)
    float Ta, Tb, Tc;
    switch (sector) {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
    }

    // Ta, Tb, Tc range [0,1]

    _writeDutyCycle3PWM(Ta, Tb, Tc);
}

float FOC_get_mechanical_angle() {
    return AS5600_ReadSensorRawData() / SENSOR_VALUE_RANGE * _2PI;
}

float FOC_electrical_angle() {

    return _normalizeAngle(SENSOR_DIRECTION * POLE_PAIR * FOC_get_mechanical_angle() - zero_electrical_angle);
}

// rad/s
float FOC_get_velocity() {
    float ts = (float) (HAL_GetTick() - pre_tick) * 1e-3;
    if (ts < 1e-3) ts = 1e-3;
    pre_tick = HAL_GetTick();

    float now_mechanical_angle = FOC_get_mechanical_angle();
    float delta_angle = now_mechanical_angle - last_mechanical_angle;
    last_mechanical_angle = now_mechanical_angle;
    if (fabs(delta_angle) > _PI) {
        if (delta_angle > 0) {
            delta_angle -= _2PI;
        } else {
            delta_angle += _2PI;
        }
    }


//    return delta_angle;
    return delta_angle / ts;
}

void FOC_open_loop_voltage_control_loop(float Uq) {
    float electrical_angle = FOC_electrical_angle();
    FOC_SVPWM(Uq, 0, electrical_angle);

}

/**
 *
 * @param target_velocity unit: rad/s
 */
void FOC_velocity_control_loop(float target_velocity) {
    static float now_velocity;
    now_velocity = FOC_low_pass_filter(&lpf_velocity, FOC_get_velocity());
    float Uq = pid_get_u(&pid_velocity, target_velocity, now_velocity);
    float electrical_angle = FOC_electrical_angle();
    FOC_SVPWM(Uq, 0, electrical_angle);

//    cs_get_value();
//    printf("%d,%d,%d\n", cs_value[0], cs_value[1], cs_value[2]);
}

void FOC_position_control_loop(float target_angle) {
    target_angle = _normalizeAngle(target_angle);
    float now_angle = _normalizeAngle(FOC_get_mechanical_angle() - zero_mechanical_angle);
//    float now_angle = FOC_get_mechanical_angle();

    float angle_error = target_angle - now_angle;
    if (angle_error < -_PI) target_angle += _2PI;
    else if (angle_error > _PI) target_angle -= _2PI;

    float target_velocity = pid_get_u(&pid_position, target_angle, now_angle);
    float now_velocity = FOC_low_pass_filter(&lpf_velocity, FOC_get_velocity());
    float Uq = pid_get_u(&pid_velocity, target_velocity, now_velocity);
    float electrical_angle = FOC_electrical_angle();
    FOC_SVPWM(Uq, 0, electrical_angle);

    //printf("%.2f,%.2f,%.2f,%.2f\n", target_angle, now_angle, target_velocity, now_velocity);
}

void FOC_spring_loop(float target_angle, PID_Datatype *pid) {
    target_angle = _normalizeAngle(target_angle);
    float now_angle = _normalizeAngle(FOC_get_mechanical_angle() - zero_mechanical_angle);

    float angle_error = target_angle - now_angle;
    if (angle_error < -_PI) target_angle += _2PI;
    else if (angle_error > _PI) target_angle -= _2PI;

    float Uq = FOC_low_pass_filter(&lpf_spring, pid_get_u(pid, target_angle, now_angle));
    float electrical_angle = FOC_electrical_angle();
    FOC_SVPWM(Uq, 0, electrical_angle);
}

void FOC_knob_loop(uint8_t sector_num) {
    float now_angle = _normalizeAngle(FOC_get_mechanical_angle() - zero_mechanical_angle);
    uint8_t now_sector = (uint8_t) floor(now_angle * sector_num / _2PI);
    float target_angle = now_sector * _2PI / sector_num + _PI / sector_num;

    float angle_error = target_angle - now_angle;
    if (angle_error < -_PI) target_angle += _2PI;
    else if (angle_error > _PI) target_angle -= _2PI;

    float Uq = pid_get_u(&pid_knob, target_angle, now_angle);

    float electrical_angle = FOC_electrical_angle();
    FOC_SVPWM(Uq, 0, electrical_angle);

    char data = now_sector + '0';

    HAL_UART_Transmit(&huart1, &data, 1, 0xff);
}

