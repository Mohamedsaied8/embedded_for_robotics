/**
 ******************************************************************************
 * @file    pid.h
 * @brief   Generic PID Controller Interface
 ******************************************************************************
 *
 * Features:
 *   - Configurable Kp, Ki, Kd gains
 *   - Anti-windup with integral clamping
 *   - Output saturation
 *   - Derivative on measurement to reduce kick
 *
 ******************************************************************************
 */

#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  PID Controller structure
 */
typedef struct {
  float Kp; /* Proportional gain */
  float Ki; /* Integral gain */
  float Kd; /* Derivative gain */

  float integral;         /* Integral accumulator */
  float prev_measurement; /* Previous measurement (for derivative on
                             measurement) */

  float integral_limit; /* Anti-windup: max integral value */
  float output_min;     /* Output saturation minimum */
  float output_max;     /* Output saturation maximum */
} PID_t;

/**
 * @brief  Initialize PID controller with gains and limits
 * @param  pid: Pointer to PID structure
 * @param  Kp: Proportional gain
 * @param  Ki: Integral gain
 * @param  Kd: Derivative gain
 * @param  output_min: Minimum output value
 * @param  output_max: Maximum output value
 */
void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float output_min,
              float output_max);

/**
 * @brief  Compute PID output
 * @param  pid: Pointer to PID structure
 * @param  setpoint: Desired value
 * @param  measurement: Actual measured value
 * @param  dt: Time delta in seconds
 * @retval PID output (clamped to min/max)
 */
float PID_Compute(PID_t *pid, float setpoint, float measurement, float dt);

/**
 * @brief  Reset PID controller state
 * @param  pid: Pointer to PID structure
 */
void PID_Reset(PID_t *pid);

/**
 * @brief  Set new PID gains
 * @param  pid: Pointer to PID structure
 * @param  Kp, Ki, Kd: New gains
 */
void PID_SetGains(PID_t *pid, float Kp, float Ki, float Kd);

/**
 * @brief  Set integral limit for anti-windup
 * @param  pid: Pointer to PID structure
 * @param  limit: Maximum integral value
 */
void PID_SetIntegralLimit(PID_t *pid, float limit);

#ifdef __cplusplus
}
#endif

#endif /* PID_H */
