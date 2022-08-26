#include "FOC_PID.h"
#include "math.h"

PID_Datatype pid_velocity;
PID_Datatype pid_position;

void pid_set_parameters() {
    pid_velocity.Kp = 0.1;
    pid_velocity.Ki = 0.01; // 0.01
    pid_velocity.Kd = 0;
    pid_velocity.integral = 0;
    pid_velocity.last_error = 0;
    pid_velocity.max_integral = 500.0; // 200
    pid_velocity.min_integral = -500.0;
    pid_velocity.max_u = 8.0;
    pid_velocity.min_u = -8.0;
    pid_velocity.dead_zone = 0;

    pid_position.Kp = 20.0;
    pid_position.Ki = 0.0;
    pid_position.Kd = 0;
    pid_position.integral = 0;
    pid_position.last_error = 0;
    pid_position.max_integral = 200.0;
    pid_position.min_integral = -200.0;
    pid_position.max_u = 100.0;
    pid_position.min_u = -100.0;
    pid_position.dead_zone = 0;

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

    if(fabs(error) < pid->dead_zone){
        error = 0; // 死区
    }


    pid->integral += error; // 积分误差
    if (pid->integral > pid->max_integral) { // 设置积分误差的上下限，防止过大或过小
        pid->integral = pid->max_integral;
    } else if (pid->integral < pid->min_integral) {
        pid->integral = pid->min_integral;
    }

    double differential = error - pid->last_error; // 差分误差

    double u = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * differential;  // 控制量
    if (u > pid->max_u) { // 设置控制量u的上下限，防止过大或过小
        u = pid->max_u;
    } else if (u < pid->min_u) {
        u = pid->min_u;
    }

    pid->last_error = error; // 记录下本轮误差供下次差分使用

    return u;
}
