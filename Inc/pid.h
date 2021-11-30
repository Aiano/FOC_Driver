//
// Created by Aiano on 2021/10/21.
//

#ifndef SELF_BALANCE_CAR_PID_H
#define SELF_BALANCE_CAR_PID_H

// pid options:change if necessary
//#define USE_P
//#define USE_I
//#define USE_D

/**
 * @brief data structure of a incremental pid controller
 */
struct pid_parameters {
    float U_k_1 = 0; // Uk-1
    float e_k_1 = 0; // ek-1
    float e_k_2 = 0; // ek-2
    float Kp = 1;
    float Ki = 1;
    float Kd = 1;
    bool use_p;
    bool use_i;
    bool use_d;
};

/**
 * @brief calculate pid control signal
 * @param in set value
 * @param out feedback value
 * @param pidParameters
 * @return u pid control signal
 */
float pid_u(float in, float out, pid_parameters &pidParameters);

#endif //SELF_BALANCE_CAR_PID_H
