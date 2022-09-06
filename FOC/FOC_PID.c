#include "FOC_PID.h"
#include "math.h"

PID_Datatype pid_velocity;
PID_Datatype pid_position;
PID_Datatype pid_spring;
PID_Datatype pid_spring_with_damp;
PID_Datatype pid_damp;
PID_Datatype pid_zero_resistance;
PID_Datatype pid_knob;

void pid_set_parameters() {
    pid_velocity.Kp = 0.08;
    pid_velocity.Ki = 0.03; // 0.03
    pid_velocity.Kd = 0;
    pid_velocity.integral = 0;
    pid_velocity.last_error = 0;
    pid_velocity.max_integral = 500.0; // 200
    pid_velocity.min_integral = -500.0;
    pid_velocity.max_u = 8.0;
    pid_velocity.min_u = -8.0;
    pid_velocity.dead_zone = 0;

    pid_position.Kp = 10.0; // 20.0
    pid_position.Ki = 0.0;
    pid_position.Kd = 0;
    pid_position.integral = 0;
    pid_position.last_error = 0;
    pid_position.max_integral = 0.0;
    pid_position.min_integral = -0.0;
    pid_position.max_u = 20.0;
    pid_position.min_u = -20.0;
    pid_position.dead_zone = 0.0;

    pid_spring.Kp = 2.5;
    pid_spring.Ki = 0.0;
    pid_spring.Kd = -32;
    pid_spring.integral = 0;
    pid_spring.last_error = 0;
    pid_spring.max_integral = 0.0;
    pid_spring.min_integral = -0.0;
    pid_spring.max_u = 2.0;
    pid_spring.min_u = -2.0;
    pid_spring.dead_zone = 0.0;

    pid_spring_with_damp.Kp = 2.5;
    pid_spring_with_damp.Ki = 0.0;
    pid_spring_with_damp.Kd = 150;
    pid_spring_with_damp.integral = 0;
    pid_spring_with_damp.last_error = 0;
    pid_spring_with_damp.max_integral = 0.0;
    pid_spring_with_damp.min_integral = -0.0;
    pid_spring_with_damp.max_u = 2.0;
    pid_spring_with_damp.min_u = -2.0;
    pid_spring_with_damp.dead_zone = 0.0;

    pid_damp.Kp = 0.0;
    pid_damp.Ki = 0.0;
    pid_damp.Kd = 200;
    pid_damp.integral = 0;
    pid_damp.last_error = 0;
    pid_damp.max_integral = 0.0;
    pid_damp.min_integral = -0.0;
    pid_damp.max_u = 8.0;
    pid_damp.min_u = -8.0;
    pid_damp.dead_zone = 0.0;

    pid_zero_resistance.Kp = 0.0;
    pid_zero_resistance.Ki = 0.0;
    pid_zero_resistance.Kd = -30.5;
    pid_zero_resistance.integral = 0;
    pid_zero_resistance.last_error = 0;
    pid_zero_resistance.max_integral = 0.0;
    pid_zero_resistance.min_integral = -0.0;
    pid_zero_resistance.max_u = 2.0;
    pid_zero_resistance.min_u = -2.0;
    pid_zero_resistance.dead_zone = 0.0;

    pid_knob.Kp = 5.0;
    pid_knob.Ki = 0.0;
    pid_knob.Kd = 0;
    pid_knob.integral = 0;
    pid_knob.last_error = 0;
    pid_knob.max_integral = 0.0;
    pid_knob.min_integral = -0.0;
    pid_knob.max_u = 4.0;
    pid_knob.min_u = -4.0;
    pid_knob.dead_zone = 0.0;
}

/**
 * @brief 获取pid的控制量u
 * @param pid
 * @param target
 * @param real
 * @return u
 */
float pid_get_u(PID_Datatype *pid, float target, float real) {
    float error = target - real; // 误差

    if (fabs(error) < pid->dead_zone) error = 0; // 死区

    pid->integral += error; // 积分误差
//    if (fabs(pid->integral) < pid->dead_zone) pid->integral = 0; // 死区

    if (pid->integral > pid->max_integral) { // 设置积分误差的上下限，防止过大或过小
        pid->integral = pid->max_integral;
    } else if (pid->integral < pid->min_integral) {
        pid->integral = pid->min_integral;
    }

    double differential = error - pid->last_error; // 差分误差
//    if (fabs(differential) < pid->dead_zone) differential = 0; // 死区

    double u = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * differential;  // 控制量
    if (u > pid->max_u) { // 设置控制量u的上下限，防止过大或过小
        u = pid->max_u;
    } else if (u < pid->min_u) {
        u = pid->min_u;
    }

    pid->last_error = error; // 记录下本轮误差供下次差分使用

    return u;
}
