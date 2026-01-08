/**
 ******************************************************************************
 * @file    differential_drive.c
 * @brief   Differential Drive Controller for Straight-Line Motion
 ******************************************************************************
 *
 * Control Strategy:
 *   - Speed PID: Controls base forward speed
 *   - Heading PID: Corrects steering to maintain straight line
 *
 * Motor outputs:
 *   Left Motor  = Base Speed - Heading Correction
 *   Right Motor = Base Speed + Heading Correction
 *
 ******************************************************************************
 */

#include "differential_drive.h"
#include "encoder.h"
#include "imu.h"
#include "main.h"
#include "motor.h"
#include "pid.h"

/* ================ Private Variables ================ */

/* PID controllers */
static PID_t speed_pid_left;
static PID_t speed_pid_right;
static PID_t heading_pid;

/* Drive state */
static DriveState_t drive_state = DRIVE_STATE_STOPPED;

/* Target values */
static float target_speed = 0.0f;

/* Current measured values */
static float current_speed_left = 0.0f;
static float current_speed_right = 0.0f;
static float current_heading = 0.0f;

/* Motor output values */
static int16_t left_motor_output = 0;
static int16_t right_motor_output = 0;

/* ================ Public Functions ================ */

/**
 * @brief  Initialize differential drive controller
 */
void DifferentialDrive_Init(void) {
  /* Initialize hardware */
  Motor_Init();
  Encoder_Init();

  /* Initialize IMU - retry if fails */
  int8_t imu_status = IMU_Init();
  (void)imu_status; /* TODO: handle IMU init failure */

  /* Initialize Speed PID for left wheel */
  PID_Init(&speed_pid_left, SPEED_PID_KP, SPEED_PID_KI, SPEED_PID_KD,
           -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);

  /* Initialize Speed PID for right wheel */
  PID_Init(&speed_pid_right, SPEED_PID_KP, SPEED_PID_KI, SPEED_PID_KD,
           -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);

  /* Initialize Heading PID */
  /* Output is a differential adjustment to motor speeds */
  PID_Init(&heading_pid, HEADING_PID_KP, HEADING_PID_KI, HEADING_PID_KD, -500,
           500); /* Max ±50% correction */

  /* Set integral limits */
  PID_SetIntegralLimit(&speed_pid_left, 300);
  PID_SetIntegralLimit(&speed_pid_right, 300);
  PID_SetIntegralLimit(&heading_pid, 200);

  drive_state = DRIVE_STATE_STOPPED;
  target_speed = 0.0f;
}

/**
 * @brief  Calibrate sensors
 */
void DifferentialDrive_Calibrate(void) {
  drive_state = DRIVE_STATE_CALIBRATING;

  /* Stop motors during calibration */
  Motor_Stop();

  /* Calibrate IMU gyro bias */
  IMU_Calibrate();

  /* Reset encoders and heading */
  Encoder_Reset();
  IMU_ResetHeading();

  /* Reset PIDs */
  PID_Reset(&speed_pid_left);
  PID_Reset(&speed_pid_right);
  PID_Reset(&heading_pid);

  drive_state = DRIVE_STATE_STOPPED;
}

/**
 * @brief  Set target forward speed
 */
void DifferentialDrive_SetSpeed(float speed) {
  target_speed = speed;

  if (speed != 0.0f) {
    if (drive_state == DRIVE_STATE_STOPPED) {
      /* Reset heading when starting to move */
      IMU_ResetHeading();
      PID_Reset(&heading_pid);
    }
    drive_state = DRIVE_STATE_RUNNING;
  } else {
    drive_state = DRIVE_STATE_STOPPED;
  }
}

/**
 * @brief  Update control loop
 */
void DifferentialDrive_Update(float dt) {
  if (dt <= 0.0f || drive_state != DRIVE_STATE_RUNNING) {
    if (drive_state == DRIVE_STATE_STOPPED) {
      Motor_Stop();
    }
    return;
  }

  /* Update IMU */
  IMU_Update(dt);

  /* Get current wheel speeds */
  current_speed_left = Encoder_GetSpeedLeft(dt);
  current_speed_right = Encoder_GetSpeedRight(dt);

  /* Get current heading */
  current_heading = IMU_GetHeading();

  /* Compute speed PID outputs */
  float speed_output_left =
      PID_Compute(&speed_pid_left, target_speed, current_speed_left, dt);
  float speed_output_right =
      PID_Compute(&speed_pid_right, target_speed, current_speed_right, dt);

  /* Compute heading correction */
  /* Setpoint is 0 (straight line), measurement is current heading */
  float heading_correction =
      PID_Compute(&heading_pid, 0.0f, current_heading, dt);

  /* Apply heading correction to motor outputs */
  /* Positive heading means robot turned right, so we need to turn left */
  /* Turn left = slow down left wheel or speed up right wheel */
  left_motor_output = (int16_t)(speed_output_left - heading_correction);
  right_motor_output = (int16_t)(speed_output_right + heading_correction);

  /* Clamp to motor limits */
  if (left_motor_output > MOTOR_MAX_SPEED)
    left_motor_output = MOTOR_MAX_SPEED;
  if (left_motor_output < MOTOR_MIN_SPEED)
    left_motor_output = MOTOR_MIN_SPEED;
  if (right_motor_output > MOTOR_MAX_SPEED)
    right_motor_output = MOTOR_MAX_SPEED;
  if (right_motor_output < MOTOR_MIN_SPEED)
    right_motor_output = MOTOR_MIN_SPEED;

  /* Apply to motors */
  Motor_SetBoth(left_motor_output, right_motor_output);
}

/**
 * @brief  Stop the robot
 */
void DifferentialDrive_Stop(void) {
  drive_state = DRIVE_STATE_STOPPED;
  target_speed = 0.0f;
  Motor_Stop();

  PID_Reset(&speed_pid_left);
  PID_Reset(&speed_pid_right);
  PID_Reset(&heading_pid);
}

/**
 * @brief  Get current state
 */
DriveState_t DifferentialDrive_GetState(void) { return drive_state; }

/**
 * @brief  Set speed PID gains
 */
void DifferentialDrive_SetSpeedPID(float Kp, float Ki, float Kd) {
  PID_SetGains(&speed_pid_left, Kp, Ki, Kd);
  PID_SetGains(&speed_pid_right, Kp, Ki, Kd);
}

/**
 * @brief  Set heading PID gains
 */
void DifferentialDrive_SetHeadingPID(float Kp, float Ki, float Kd) {
  PID_SetGains(&heading_pid, Kp, Ki, Kd);
}

/**
 * @brief  Get current average speed
 */
float DifferentialDrive_GetCurrentSpeed(void) {
  return (current_speed_left + current_speed_right) / 2.0f;
}

/**
 * @brief  Get heading error
 */
float DifferentialDrive_GetHeadingError(void) { return current_heading; }
