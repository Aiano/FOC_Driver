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


#endif //FOC_DRIVER_FOC_CONF_H
