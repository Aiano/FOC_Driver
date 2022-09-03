//
// Created by Aiano on 2022/9/3.
//

#include "FOC_LPF.h"

LPF_Datatype lpf_velocity;
LPF_Datatype lpf_spring;


void FOC_lpf_set_parameters() {
    lpf_velocity.last_output = 0;
    lpf_velocity.alpha = 0.5;

    lpf_spring.last_output = 0;
    lpf_spring.alpha = 0.2;
}

float FOC_low_pass_filter(LPF_Datatype *lpf, float input) {
    float output = lpf->alpha * input + (1 - lpf->alpha) * lpf->last_output;
    lpf->last_output = output;

    return output;
}
