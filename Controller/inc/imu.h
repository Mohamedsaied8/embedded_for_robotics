/**
 ******************************************************************************
 * @file    imu.h
 * @brief   IMU (MPU6050) Driver Interface
 ******************************************************************************
 *
 * Hardware Setup:
 *   - I2C1_SDA -> PB7
 *   - I2C1_SCL -> PB6
 *   - MPU6050 Address: 0x68 (AD0 = GND) or 0x69 (AD0 = VCC)
 *
 ******************************************************************************
 */

#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* MPU6050 I2C Address (AD0 = GND) */
#define MPU6050_ADDR 0x68

/* MPU6050 Register Addresses */
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_SMPLRT_DIV 0x19
#define MPU6050_REG_CONFIG 0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_GYRO_XOUT_H 0x43
#define MPU6050_REG_GYRO_YOUT_H 0x45
#define MPU6050_REG_GYRO_ZOUT_H 0x47
#define MPU6050_REG_WHO_AM_I 0x75

/**
 * @brief  IMU data structure
 */
typedef struct {
  float gyro_x; /* Gyro X in deg/s */
  float gyro_y; /* Gyro Y in deg/s */
  float gyro_z; /* Gyro Z in deg/s (yaw rate) */

  float heading; /* Integrated heading in degrees */

  float gyro_z_bias; /* Gyro Z bias (calibrated) */
} IMU_Data_t;

/**
 * @brief  Initialize IMU (MPU6050) over I2C1
 * @retval 0 on success, -1 on failure
 */
int8_t IMU_Init(void);

/**
 * @brief  Calibrate gyroscope (robot must be stationary)
 * @note   Samples gyro for ~1 second to calculate bias
 */
void IMU_Calibrate(void);

/**
 * @brief  Update IMU readings and integrate heading
 * @param  dt: Time delta in seconds
 */
void IMU_Update(float dt);

/**
 * @brief  Get current yaw rate (Z-axis rotation)
 * @retval Yaw rate in deg/s
 */
float IMU_GetYawRate(void);

/**
 * @brief  Get integrated heading
 * @retval Heading in degrees (accumulated)
 */
float IMU_GetHeading(void);

/**
 * @brief  Reset heading to zero
 */
void IMU_ResetHeading(void);

/**
 * @brief  Get pointer to IMU data structure
 * @retval Pointer to IMU_Data_t
 */
IMU_Data_t *IMU_GetData(void);

#ifdef __cplusplus
}
#endif

#endif /* IMU_H */
