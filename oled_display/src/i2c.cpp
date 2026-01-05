/**
  ******************************************************************************
  * @file    i2c.cpp
  * @brief   I2C driver class implementation for STM32F103
  ******************************************************************************
  */

#include "i2c.hpp"

// I2C Timeout in milliseconds
#define I2C_TIMEOUT 100

I2C::I2C(uint8_t address) : m_address(address), m_hi2c{} {
}

void I2C::configureGPIO() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIOB clock
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // Configure PB6 (SCL) and PB7 (SDA) as alternate function open-drain
    GPIO_InitStruct.Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C1_GPIO_PORT, &GPIO_InitStruct);
}

void I2C::init() {
    // Enable I2C1 clock
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    // Configure GPIO pins
    configureGPIO();
    
    // Configure I2C1
    m_hi2c.Instance = I2C1;
    m_hi2c.Init.ClockSpeed = 100000;  // 100kHz standard mode
    m_hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    m_hi2c.Init.OwnAddress1 = 0;
    m_hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    m_hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    m_hi2c.Init.OwnAddress2 = 0;
    m_hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    m_hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    HAL_I2C_Init(&m_hi2c);
}

bool I2C::writeReg(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&m_hi2c, 
                                                        m_address << 1, 
                                                        buf, 2, 
                                                        I2C_TIMEOUT);
    return (status == HAL_OK);
}

bool I2C::writeData(uint8_t reg, const uint8_t* data, uint16_t len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&m_hi2c,
                                                  m_address << 1,
                                                  reg,
                                                  I2C_MEMADD_SIZE_8BIT,
                                                  const_cast<uint8_t*>(data),
                                                  len,
                                                  I2C_TIMEOUT);
    return (status == HAL_OK);
}

bool I2C::writeCmd(uint8_t cmd) {
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&m_hi2c,
                                                        m_address << 1,
                                                        &cmd, 1,
                                                        I2C_TIMEOUT);
    return (status == HAL_OK);
}

bool I2C::readReg(uint8_t reg, uint8_t* data) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&m_hi2c,
                                                 m_address << 1,
                                                 reg,
                                                 I2C_MEMADD_SIZE_8BIT,
                                                 data, 1,
                                                 I2C_TIMEOUT);
    return (status == HAL_OK);
}
