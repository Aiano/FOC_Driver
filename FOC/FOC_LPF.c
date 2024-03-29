//
// Created by Aiano on 2022/9/3.
//

#include "FOC_LPF.h"

LPF_Datatype lpf_velocity;
LPF_Datatype lpf_spring;
LPF_Datatype lpf_cs[3];
LPF_Datatype lpf_current_d;
LPF_Datatype lpf_current_q;


void FOC_lpf_set_parameters() {
    lpf_velocity.last_output = 0;
    lpf_velocity.alpha = 0.5;

    lpf_spring.last_output = 0;
    lpf_spring.alpha = 0.2;

    for (int i = 0; i < 3; ++i) {
        lpf_cs[i].last_output = 0;
        lpf_cs[i].alpha = 0.1;
    }

    lpf_current_d.last_output = 0;
    lpf_current_d.alpha = 0.2;

    lpf_current_q.last_output = 0;
    lpf_current_q.alpha = 0.2;
}

float FOC_low_pass_filter(LPF_Datatype *lpf, float input) {
    float output = lpf->alpha * input + (1 - lpf->alpha) * lpf->last_output;
    lpf->last_output = output;

    return output;
}
