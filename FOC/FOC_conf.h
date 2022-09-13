/**
 * @file FOC_conf.h
 * @brief configurations of FOC algorithm
 */

#ifndef FOC_DRIVER_FOC_CONF_H
#define FOC_DRIVER_FOC_CONF_H

// Driver configuration
#define VOLTAGE_LIMIT 8

// BLDC motor configuration
#define POLE_PAIR 7
#define ZERO_ELECTRICAL_ANGLE 0.85f

// Magnetic sensor configuration
#define SENSOR_VALUE_RANGE 4095
#define SENSOR_DIRECTION 1

// Control mode, uncomment one at a time
//#define FOC_MODE_VOLTAGE_CONTROL
#define FOC_MODE_CURRENT_CONTROL

#endif //FOC_DRIVER_FOC_CONF_H
