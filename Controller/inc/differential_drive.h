/**
 ******************************************************************************
 * @file    differential_drive.h
 * @brief   Differential Drive Controller for Straight-Line Motion
 ******************************************************************************
 *
 * Control Strategy:
 *   1. Speed PID - Controls base forward speed using average encoder feedback
 *   2. Heading PID - Corrects steering using IMU heading to maintain straight
 * line
 *
 * The heading correction is applied as a differential to left/right motors:
 *   Left Motor  = Base Speed - Heading Correction
 *   Right Motor = Base Speed + Heading Correction
 *
 ******************************************************************************
 */

#ifndef DIFFERENTIAL_DRIVE_H
#define DIFFERENTIAL_DRIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Default PID tuning values (adjust for your robot) */
#define SPEED_PID_KP 2.0f
#define SPEED_PID_KI 0.5f
#define SPEED_PID_KD 0.1f

#define HEADING_PID_KP 5.0f
#define HEADING_PID_KI 0.1f
#define HEADING_PID_KD 0.5f

/**
 * @brief  Drive state enumeration
 */
typedef enum {
  DRIVE_STATE_STOPPED,
  DRIVE_STATE_RUNNING,
  DRIVE_STATE_CALIBRATING
} DriveState_t;

/**
 * @brief  Initialize differential drive controller
 */
void DifferentialDrive_Init(void);

/**
 * @brief  Set target forward speed
 * @param  speed: Target speed in encoder counts per second
 */
void DifferentialDrive_SetSpeed(float speed);

/**
 * @brief  Update control loop (call at fixed rate, e.g., 100Hz)
 * @param  dt: Time delta in seconds
 */
void DifferentialDrive_Update(float dt);

/**
 * @brief  Stop the robot
 */
void DifferentialDrive_Stop(void);

/**
 * @brief  Get current drive state
 * @retval DriveState_t
 */
DriveState_t DifferentialDrive_GetState(void);

/**
 * @brief  Calibrate sensors (IMU gyro bias)
 * @note   Robot must be stationary during calibration
 */
void DifferentialDrive_Calibrate(void);

/**
 * @brief  Set speed PID gains
 */
void DifferentialDrive_SetSpeedPID(float Kp, float Ki, float Kd);

/**
 * @brief  Set heading PID gains
 */
void DifferentialDrive_SetHeadingPID(float Kp, float Ki, float Kd);

/**
 * @brief  Get current average wheel speed
 * @retval Speed in counts/second
 */
float DifferentialDrive_GetCurrentSpeed(void);

/**
 * @brief  Get current heading error
 * @retval Heading error in degrees
 */
float DifferentialDrive_GetHeadingError(void);

#ifdef __cplusplus
}
#endif

#endif /* DIFFERENTIAL_DRIVE_H */
