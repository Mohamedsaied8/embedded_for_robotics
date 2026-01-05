# Bare-Metal Quadrature Encoder for STM32F103

A minimal bare-metal implementation of quadrature encoder interface using **direct register access** - no CMSIS, no HAL libraries.

## 🎯 Features

- ✅ **Pure bare-metal**: Direct register manipulation
- ✅ **No dependencies**: No CMSIS, no HAL, no external libraries
- ✅ **TIM2 Encoder Mode**: Hardware quadrature decoding
- ✅ **32-bit counter**: Full range position tracking
- ✅ **Educational**: Learn STM32 registers from scratch
- ✅ **Minimal code**: ~250 lines including comments

## 📌 Hardware Setup

### Pin Connections

| Pin  | Function | Description |
|------|----------|-------------|
| PA0  | TIM2_CH1 | Encoder Channel A |
| PA1  | TIM2_CH2 | Encoder Channel B |
| PC13 | LED      | Activity indicator (optional) |
| GND  | GND      | Common ground |

### Encoder Wiring

```
Encoder          STM32F103C8T6
-------          -------------
  A     -------> PA0 (TIM2_CH1)
  B     -------> PA1 (TIM2_CH2)
 GND    -------> GND
 VCC    -------> 3.3V or 5V (depending on encoder)
```

**Note**: Most rotary encoders have pull-up resistors built-in. If yours doesn't, add 10kΩ pull-ups to 3.3V on PA0 and PA1.

## 🔧 How It Works

### Encoder Mode 3

The STM32 timer peripheral has built-in quadrature encoder support. This project uses **Encoder Mode 3**:
- Counts on **both** TI1 (Channel A) and TI2 (Channel B) edges
- Provides 4x resolution (4 counts per encoder pulse)
- Automatic direction detection

### Register-Level Configuration

#### 1. **TIM2_SMCR** (Slave Mode Control Register)
```c
TIM2->SMCR = 0x0003;  // SMS[2:0] = 011 (Encoder mode 3)
```

#### 2. **TIM2_CCMR1** (Capture/Compare Mode Register)
```c
TIM2->CCMR1 = 0x0101;  // CC1S=01 (TI1), CC2S=01 (TI2)
```

#### 3. **TIM2_CCER** (Capture/Compare Enable Register)
```c
TIM2->CCER = 0x0011;  // CC1E=1, CC2E=1 (Enable both channels)
```

#### 4. **TIM2_ARR** (Auto-Reload Register)
```c
TIM2->ARR = 0xFFFFFFFF;  // 32-bit counter range
```

#### 5. **TIM2_CR1** (Control Register 1)
```c
TIM2->CR1 = 0x0001;  // CEN=1 (Enable counter)
```

## 🚀 Building

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt install cmake gcc-arm-none-eabi stlink-tools

# Arch Linux
sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-newlib stlink
```

### Build Commands

```bash
cd quadrature_encoder_baremetal

# Build
./build.sh

# Or manually
mkdir build && cd build
cmake ..
make
```

### Build Output

```
text    data     bss     dec     hex filename
 XXX      XX      XX    XXXX    XXXX quadrature_encoder_baremetal.elf
```

## 📥 Flashing

### Using st-flash (Recommended)

```bash
./scripts/flash.sh
```

### Manual Flash

```bash
st-flash --reset write build/quadrature_encoder_baremetal.bin 0x08000000
```

## 🧪 Testing

1. **Connect encoder** to PA0 (A) and PA1 (B)
2. **Flash firmware** to STM32
3. **Rotate encoder**:
   - Clockwise → Counter increases
   - Counter-clockwise → Counter decreases
4. **LED (PC13)** toggles on count changes

### Reading Encoder Value

The encoder count is stored in `TIM2->CNT` register:

```c
int32_t count = Encoder_GetCount();
```

### Resetting Counter

```c
Encoder_Reset();  // Sets TIM2->CNT = 0
```

## 📚 Register Reference

### Key Registers Used

| Register | Address | Purpose |
|----------|---------|---------|
| RCC_CR | 0x40021000 | Clock control |
| RCC_CFGR | 0x40021004 | Clock configuration |
| RCC_APB1ENR | 0x4002101C | TIM2 clock enable |
| RCC_APB2ENR | 0x40021018 | GPIO clock enable |
| GPIOA_CRL | 0x40010800 | PA0-PA7 configuration |
| GPIOC_CRH | 0x40011004 | PC8-PC15 configuration |
| TIM2_CR1 | 0x40000000 | Timer control |
| TIM2_SMCR | 0x40000008 | Encoder mode config |
| TIM2_CCMR1 | 0x40000018 | Input capture config |
| TIM2_CCER | 0x40000020 | Channel enable |
| TIM2_CNT | 0x40000024 | **Counter value** |
| TIM2_ARR | 0x4000002C | Auto-reload (max count) |

### Encoder Mode Selection

| SMS[2:0] | Mode | Description |
|----------|------|-------------|
| 001 | Encoder 1 | Count on TI1 edges only |
| 010 | Encoder 2 | Count on TI2 edges only |
| **011** | **Encoder 3** | **Count on both edges (4x)** |

## 🎓 Learning Resources

### Understanding Quadrature Encoding

```
Channel A: ___/‾‾‾\___/‾‾‾\___
Channel B: _/‾‾‾\___/‾‾‾\___/‾

Clockwise:     A leads B (A changes first)
Counter-CW:    B leads A (B changes first)
```

### Register Bit Manipulation

```c
/* Set bit */
SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

/* Clear bit */
CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);

/* Read bit */
if (READ_BIT(RCC->CR, RCC_CR_HSERDY)) { ... }

/* Modify register */
MODIFY_REG(TIM2->SMCR, 0x07, 0x03);  // Clear bits 0-2, set to 3
```

## 🔍 Code Structure

```
quadrature_encoder_baremetal/
├── CMakeLists.txt           # Build configuration
├── build.sh                 # Build script
├── inc/
│   └── stm32f103_regs.h    # Register definitions
├── src/
│   └── main.c              # Main application
├── linker/
│   └── STM32F103C8Tx_FLASH.ld
└── scripts/
    └── flash.sh            # Flash script
```

## 🐛 Troubleshooting

### Encoder Not Counting

1. **Check connections**: Verify PA0 and PA1 are connected correctly
2. **Check encoder power**: Ensure encoder has power (3.3V or 5V)
3. **Check pull-ups**: Some encoders need external pull-up resistors
4. **Swap channels**: Try swapping A and B if direction is reversed

### Counter Jumping Randomly

- **Add filtering**: Modify `TIM2_CCMR1` to enable input filter:
  ```c
  TIM2->CCMR1 |= (0x3 << 4) | (0x3 << 12);  // IC1F=3, IC2F=3
  ```

### Wrong Direction

- **Swap channels**: Either in hardware or invert polarity in `TIM2_CCER`:
  ```c
  TIM2->CCER |= TIM_CCER_CC1P;  // Invert channel 1
  ```

## 📖 References

- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf) - Chapter 15: Timers
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [Quadrature Encoder Basics](https://www.dynapar.com/technology/encoder_basics/quadrature_encoder/)

## 🎯 Next Steps

### Enhancements

1. **Add velocity calculation**: Use timer interrupts to measure RPM
2. **Add index pulse**: Use TIM2_CH3 for absolute positioning
3. **Add UART output**: Print encoder value via serial
4. **Add filtering**: Implement software debouncing
5. **Multi-encoder**: Use TIM3 and TIM4 for additional encoders

### Example: Velocity Calculation

```c
// In timer interrupt (every 100ms)
int32_t current_count = Encoder_GetCount();
int32_t delta = current_count - last_count;
float rpm = (delta * 600.0f) / (PPR * 4);  // PPR = pulses per revolution
last_count = current_count;
```

## 📝 License

This project is provided as-is for educational purposes.

## 🤝 Contributing

Feel free to submit issues and improvements!

---

**Made with ❤️ for embedded systems learners**
