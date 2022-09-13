#ifndef FOC_DRIVER_FOC_PID_H
#define FOC_DRIVER_FOC_PID_H

typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float integral;
    float last_error;

    float max_integral;
    float min_integral;

    float max_u;
    float min_u;

    float dead_zone;
} PID_Datatype;

extern PID_Datatype pid_velocity;
extern PID_Datatype pid_position;
extern PID_Datatype pid_spring;
extern PID_Datatype pid_spring_with_damp;
extern PID_Datatype pid_damp;
extern PID_Datatype pid_zero_resistance;
extern PID_Datatype pid_knob;
extern PID_Datatype pid_current_q;
extern PID_Datatype pid_current_d;

void pid_set_parameters();
float pid_get_u(PID_Datatype *pid, float target, float real);

#endif //FOC_DRIVER_FOC_PID_H
