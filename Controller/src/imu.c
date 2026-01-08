/**
 ******************************************************************************
 * @file    imu.c
 * @brief   MPU6050 IMU Driver Implementation
 ******************************************************************************
 *
 * I2C1 Configuration:
 *   - SCL: PB6
 *   - SDA: PB7
 *   - Clock: 100kHz (standard mode)
 *
 ******************************************************************************
 */

#include "imu.h"
#include "main.h"

/* ================ Private Variables ================ */

static IMU_Data_t imu_data = {0};

/* Gyroscope sensitivity: 131 LSB/(deg/s) for ±250 deg/s range */
#define GYRO_SENSITIVITY 131.0f

/* ================ Private Functions ================ */

/**
 * @brief  Initialize I2C1 peripheral
 */
static void I2C1_Init(void) {
  /* Enable clocks */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); /* GPIOB */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* AFIO */
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN); /* I2C1 */

  /* Configure PB6 (SCL) and PB7 (SDA) as Alternate Function Open-Drain */
  /* PB6: bits 24-27, PB7: bits 28-31 in CRL */
  /* MODE=11 (50MHz), CNF=11 (AF Open-Drain) = 0xF */
  MODIFY_REG(GPIOB->CRL, 0xFF000000, 0xFF000000);

  /* Reset I2C1 */
  SET_BIT(I2C1->CR1, I2C_CR1_SWRST);
  CLEAR_BIT(I2C1->CR1, I2C_CR1_SWRST);

  /* Configure I2C timing for 100kHz at 36MHz APB1 */
  /* FREQ = APB1 clock in MHz = 36 */
  WRITE_REG(I2C1->CR2, 36);

  /* CCR for standard mode: T_high = T_low = CCR * T_PCLK1 */
  /* For 100kHz: CCR = 36MHz / (2 * 100kHz) = 180 */
  WRITE_REG(I2C1->CCR, 180);

  /* TRISE = (T_rise / T_PCLK1) + 1 = (1000ns / 27.7ns) + 1 = 37 */
  WRITE_REG(I2C1->TRISE, 37);

  /* Enable I2C1 */
  SET_BIT(I2C1->CR1, I2C_CR1_PE);
}

/**
 * @brief  Generate I2C START condition
 */
static void I2C1_Start(void) {
  SET_BIT(I2C1->CR1, I2C_CR1_START);
  while (!(I2C1->SR1 & I2C_SR1_SB))
    ;
}

/**
 * @brief  Generate I2C STOP condition
 */
static void I2C1_Stop(void) { SET_BIT(I2C1->CR1, I2C_CR1_STOP); }

/**
 * @brief  Send address with R/W bit
 */
static void I2C1_SendAddress(uint8_t addr, uint8_t read) {
  I2C1->DR = (addr << 1) | (read ? 1 : 0);
  while (!(I2C1->SR1 & I2C_SR1_ADDR))
    ;
  (void)I2C1->SR2; /* Clear ADDR flag by reading SR2 */
}

/**
 * @brief  Write byte to I2C
 */
static void I2C1_WriteByte(uint8_t data) {
  I2C1->DR = data;
  while (!(I2C1->SR1 & I2C_SR1_TXE))
    ;
}

/**
 * @brief  Read byte from I2C with ACK
 */
static uint8_t I2C1_ReadByteAck(void) {
  SET_BIT(I2C1->CR1, I2C_CR1_ACK);
  while (!(I2C1->SR1 & I2C_SR1_RXNE))
    ;
  return I2C1->DR;
}

/**
 * @brief  Read byte from I2C with NACK
 */
static uint8_t I2C1_ReadByteNack(void) {
  CLEAR_BIT(I2C1->CR1, I2C_CR1_ACK);
  I2C1_Stop();
  while (!(I2C1->SR1 & I2C_SR1_RXNE))
    ;
  return I2C1->DR;
}

/**
 * @brief  Write to MPU6050 register
 */
static void MPU6050_WriteReg(uint8_t reg, uint8_t value) {
  I2C1_Start();
  I2C1_SendAddress(MPU6050_ADDR, 0); /* Write */
  I2C1_WriteByte(reg);
  I2C1_WriteByte(value);
  I2C1_Stop();
}

/**
 * @brief  Read from MPU6050 register
 */
static uint8_t MPU6050_ReadReg(uint8_t reg) {
  uint8_t value;

  I2C1_Start();
  I2C1_SendAddress(MPU6050_ADDR, 0); /* Write */
  I2C1_WriteByte(reg);

  I2C1_Start();
  I2C1_SendAddress(MPU6050_ADDR, 1); /* Read */
  value = I2C1_ReadByteNack();

  return value;
}

/**
 * @brief  Read 16-bit value from MPU6050 (big-endian)
 */
static int16_t MPU6050_ReadReg16(uint8_t reg) {
  int16_t value;

  I2C1_Start();
  I2C1_SendAddress(MPU6050_ADDR, 0); /* Write */
  I2C1_WriteByte(reg);

  I2C1_Start();
  I2C1_SendAddress(MPU6050_ADDR, 1); /* Read */
  uint8_t high = I2C1_ReadByteAck();
  uint8_t low = I2C1_ReadByteNack();

  value = (int16_t)((high << 8) | low);
  return value;
}

/* ================ Public Functions ================ */

/**
 * @brief  Initialize MPU6050
 */
int8_t IMU_Init(void) {
  /* Initialize I2C1 */
  I2C1_Init();

  /* Check WHO_AM_I register */
  uint8_t who_am_i = MPU6050_ReadReg(MPU6050_REG_WHO_AM_I);
  if (who_am_i != 0x68) {
    return -1; /* MPU6050 not found */
  }

  /* Wake up MPU6050 (clear sleep bit) */
  MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x00);

  /* Set sample rate divider: 1kHz / (1 + 9) = 100Hz */
  MPU6050_WriteReg(MPU6050_REG_SMPLRT_DIV, 9);

  /* Set DLPF to ~44Hz bandwidth */
  MPU6050_WriteReg(MPU6050_REG_CONFIG, 0x03);

  /* Set gyro range to ±250 deg/s */
  MPU6050_WriteReg(MPU6050_REG_GYRO_CONFIG, 0x00);

  /* Set accelerometer range to ±2g (not used for heading, but set anyway) */
  MPU6050_WriteReg(MPU6050_REG_ACCEL_CONFIG, 0x00);

  /* Initialize IMU data */
  imu_data.gyro_x = 0.0f;
  imu_data.gyro_y = 0.0f;
  imu_data.gyro_z = 0.0f;
  imu_data.heading = 0.0f;
  imu_data.gyro_z_bias = 0.0f;

  return 0;
}

/**
 * @brief  Calibrate gyroscope (robot must be stationary)
 */
void IMU_Calibrate(void) {
  float sum = 0.0f;
  const int samples = 100;

  for (int i = 0; i < samples; i++) {
    int16_t raw_z = MPU6050_ReadReg16(MPU6050_REG_GYRO_ZOUT_H);
    sum += (float)raw_z / GYRO_SENSITIVITY;

    /* Simple delay */
    for (volatile int j = 0; j < 10000; j++)
      ;
  }

  imu_data.gyro_z_bias = sum / samples;
}

/**
 * @brief  Update IMU readings
 */
void IMU_Update(float dt) {
  /* Read gyro Z (yaw rate) */
  int16_t raw_z = MPU6050_ReadReg16(MPU6050_REG_GYRO_ZOUT_H);

  /* Convert to deg/s and subtract bias */
  imu_data.gyro_z = ((float)raw_z / GYRO_SENSITIVITY) - imu_data.gyro_z_bias;

  /* Integrate heading */
  imu_data.heading += imu_data.gyro_z * dt;

  /* Normalize heading to -180 to +180 */
  while (imu_data.heading > 180.0f)
    imu_data.heading -= 360.0f;
  while (imu_data.heading < -180.0f)
    imu_data.heading += 360.0f;
}

/**
 * @brief  Get current yaw rate
 */
float IMU_GetYawRate(void) { return imu_data.gyro_z; }

/**
 * @brief  Get integrated heading
 */
float IMU_GetHeading(void) { return imu_data.heading; }

/**
 * @brief  Reset heading to zero
 */
void IMU_ResetHeading(void) { imu_data.heading = 0.0f; }

/**
 * @brief  Get pointer to IMU data
 */
IMU_Data_t *IMU_GetData(void) { return &imu_data; }
