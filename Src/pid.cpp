//
// Created by Aiano on 2021/10/21.
//

#include "pid.h"


float pid_u(float in, float out, pid_parameters &pidParameters) {
    // e(k)
    float nowErr = in - out;

    // deltaU
    float deltaU = 0;
    if (pidParameters.use_p)
        deltaU += pidParameters.Kp * (nowErr - pidParameters.e_k_1);
    if (pidParameters.use_i)
        deltaU += pidParameters.Ki * nowErr;
    if (pidParameters.use_d)
        deltaU += pidParameters.Kd * (nowErr - 2 * pidParameters.e_k_1 + pidParameters.e_k_2);

    // U(k)
    float nowU = deltaU + pidParameters.U_k_1;

    // update pid parameters
    pidParameters.U_k_1 = nowU;
    pidParameters.e_k_2 = pidParameters.e_k_1;
    pidParameters.e_k_1 = nowErr;

    return nowU;
}
