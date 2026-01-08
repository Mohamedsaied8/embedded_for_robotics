/**
 ******************************************************************************
 * @file    pid.c
 * @brief   Generic PID Controller Implementation
 ******************************************************************************
 */

#include "pid.h"

/* ================ Private Functions ================ */

/**
 * @brief  Clamp value between min and max
 */
static float clamp(float value, float min, float max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

/* ================ Public Functions ================ */

/**
 * @brief  Initialize PID controller
 */
void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float output_min,
              float output_max) {
  pid->Kp = Kp;
  pid->Ki = Ki;
  pid->Kd = Kd;

  pid->integral = 0.0f;
  pid->prev_measurement = 0.0f;

  pid->integral_limit = output_max * 0.5f; /* Default: 50% of max output */
  pid->output_min = output_min;
  pid->output_max = output_max;
}

/**
 * @brief  Compute PID output using derivative on measurement
 * @note   Using derivative on measurement instead of derivative on error
 *         to reduce "derivative kick" when setpoint changes
 */
float PID_Compute(PID_t *pid, float setpoint, float measurement, float dt) {
  if (dt <= 0.0f) {
    return 0.0f;
  }

  /* Calculate error */
  float error = setpoint - measurement;

  /* Proportional term */
  float P = pid->Kp * error;

  /* Integral term with anti-windup */
  pid->integral += pid->Ki * error * dt;
  pid->integral =
      clamp(pid->integral, -pid->integral_limit, pid->integral_limit);
  float I = pid->integral;

  /* Derivative term (on measurement to avoid kick) */
  float dmeasurement = (measurement - pid->prev_measurement) / dt;
  float D = -pid->Kd *
            dmeasurement; /* Negative because we use measurement, not error */
  pid->prev_measurement = measurement;

  /* Calculate output */
  float output = P + I + D;

  /* Apply output saturation */
  output = clamp(output, pid->output_min, pid->output_max);

  /* Anti-windup: if output saturated, stop integrating in that direction */
  if ((output >= pid->output_max && error > 0) ||
      (output <= pid->output_min && error < 0)) {
    pid->integral -= pid->Ki * error * dt; /* Undo the integration */
  }

  return output;
}

/**
 * @brief  Reset PID controller state
 */
void PID_Reset(PID_t *pid) {
  pid->integral = 0.0f;
  pid->prev_measurement = 0.0f;
}

/**
 * @brief  Set new PID gains
 */
void PID_SetGains(PID_t *pid, float Kp, float Ki, float Kd) {
  pid->Kp = Kp;
  pid->Ki = Ki;
  pid->Kd = Kd;
}

/**
 * @brief  Set integral limit for anti-windup
 */
void PID_SetIntegralLimit(PID_t *pid, float limit) {
  pid->integral_limit = limit;
  /* Clamp existing integral if necessary */
  pid->integral = clamp(pid->integral, -limit, limit);
}
