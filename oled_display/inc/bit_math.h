#pragma once

#define SET_BIT(REG, BIT)       ((REG) |= (1 << (BIT)))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1 << (BIT)))
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (1 << (BIT)))
#define READ_BIT(REG, BIT)      ((REG) & (1 << (BIT)))