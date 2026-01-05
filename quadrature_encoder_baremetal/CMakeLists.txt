cmake_minimum_required(VERSION 3.15)

# Set the toolchain file before project()
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/arm-none-eabi-gcc.cmake)

# Project name and languages
project(quadrature_encoder C ASM)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# MCU specific settings
set(MCU_FAMILY STM32F1xx)
set(MCU_MODEL STM32F103xB)
set(MCU_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F103C8Tx_FLASH.ld)

# STM32Cube paths
set(CUBE_ROOT ${CMAKE_SOURCE_DIR}/../STM32Cube_FW_F1_V1.8.0)
set(CMSIS_DIR ${CUBE_ROOT}/Drivers/CMSIS)
set(HAL_DIR ${CUBE_ROOT}/Drivers/STM32F1xx_HAL_Driver)

# Compiler options
set(MCU_FLAGS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Linker options
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -T${MCU_LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map --specs=nano.specs --specs=nosys.specs")

# Definitions
add_compile_definitions(
    ${MCU_MODEL}
    USE_HAL_DRIVER
    HSE_VALUE=8000000
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/CMSIS_5/CMSIS/Core/Include
    ${CMSIS_DIR}/Device/ST/STM32F1xx/Include
    ${HAL_DIR}/Inc
    ${HAL_DIR}/Inc/Legacy
)

# HAL source files
file(GLOB HAL_SOURCES
    ${HAL_DIR}/Src/stm32f1xx_hal.c
    ${HAL_DIR}/Src/stm32f1xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32f1xx_hal_dma.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_uart.c
)

# Common source files (HAL support)
set(COMMON_SOURCES
    ${CMAKE_SOURCE_DIR}/src/stm32f1xx_it.c
    ${CMAKE_SOURCE_DIR}/src/stm32f1xx_hal_msp.c
    ${CMAKE_SOURCE_DIR}/src/system_stm32f1xx.c
    ${CMAKE_SOURCE_DIR}/startup/startup_stm32f103xb.s
)

# ==================== Timer Encoder Version ====================
add_executable(encoder_timer.elf
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/uart.c
    ${COMMON_SOURCES}
    ${HAL_SOURCES}
)
set_target_properties(encoder_timer.elf PROPERTIES
    LINK_FLAGS "-Wl,-Map=encoder_timer.map"
)

add_custom_command(TARGET encoder_timer.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex encoder_timer.elf encoder_timer.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary encoder_timer.elf encoder_timer.bin
    COMMAND ${CMAKE_SIZE} encoder_timer.elf
    COMMENT "Timer encoder: Building HEX and BIN"
)

# ==================== EXTI Encoder Version ====================
add_executable(encoder_exti.elf
    ${CMAKE_SOURCE_DIR}/src/main_exti.c
    ${CMAKE_SOURCE_DIR}/src/encoder.c
    ${CMAKE_SOURCE_DIR}/src/uart.c
    ${COMMON_SOURCES}
    ${HAL_SOURCES}
)
set_target_properties(encoder_exti.elf PROPERTIES
    LINK_FLAGS "-Wl,-Map=encoder_exti.map"
)

add_custom_command(TARGET encoder_exti.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex encoder_exti.elf encoder_exti.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary encoder_exti.elf encoder_exti.bin
    COMMAND ${CMAKE_SIZE} encoder_exti.elf
    COMMENT "EXTI encoder: Building HEX and BIN"
)

# Print build info
message(STATUS "MCU: ${MCU_MODEL}")
message(STATUS "Linker script: ${MCU_LINKER_SCRIPT}")
message(STATUS "Build targets: encoder_timer.elf, encoder_exti.elf")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
