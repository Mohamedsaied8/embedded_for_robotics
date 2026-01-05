/**
  ******************************************************************************
  * @file    i2c.hpp
  * @brief   I2C driver class for STM32F103 bare-metal OOP approach
  ******************************************************************************
  */

#ifndef __I2C_HPP
#define __I2C_HPP

#include "main.h"
#include <cstdint>

/**
 * @brief I2C Master class for bare-metal communication
 * 
 * This class provides an OOP interface to the STM32F103 I2C1 peripheral.
 * Uses PB6 (SCL) and PB7 (SDA) pins in alternate function open-drain mode.
 */
class I2C {
public:
    /**
     * @brief Construct a new I2C object
     * @param address 7-bit I2C slave address
     */
    explicit I2C(uint8_t address);
    
    /**
     * @brief Initialize I2C1 peripheral
     * Configures GPIO pins and I2C registers for 100kHz standard mode
     */
    void init();
    
    /**
     * @brief Write a single byte to a register
     * @param reg Register address
     * @param data Data byte to write
     * @return true if successful, false on error
     */
    bool writeReg(uint8_t reg, uint8_t data);
    
    /**
     * @brief Write multiple bytes starting at a register
     * @param reg Starting register address
     * @param data Pointer to data buffer
     * @param len Number of bytes to write
     * @return true if successful, false on error
     */
    bool writeData(uint8_t reg, const uint8_t* data, uint16_t len);
    
    /**
     * @brief Write command byte (no register address)
     * @param cmd Command byte
     * @return true if successful, false on error
     */
    bool writeCmd(uint8_t cmd);
    
    /**
     * @brief Read a single byte from a register
     * @param reg Register address
     * @param data Pointer to store read byte
     * @return true if successful, false on error
     */
    bool readReg(uint8_t reg, uint8_t* data);

private:
    uint8_t m_address;  ///< 7-bit slave address
    I2C_HandleTypeDef m_hi2c;  ///< HAL I2C handle
    
    /**
     * @brief Configure GPIO pins for I2C1
     */
    void configureGPIO();
};

#endif /* __I2C_HPP */
