//
// Created by Aiano on 2022/9/3.
//

#ifndef FOC_DRIVER_FOC_LPF_H
#define FOC_DRIVER_FOC_LPF_H

typedef struct {
    float alpha;
    float last_output;
}LPF_Datatype;

extern LPF_Datatype lpf_velocity;
extern LPF_Datatype lpf_spring;

void FOC_lpf_set_parameters();
float FOC_low_pass_filter(LPF_Datatype *lpf, float input);
#endif //FOC_DRIVER_FOC_LPF_H
