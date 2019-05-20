#include <stdint.h>
#include "stm32f10x.h"

#ifndef DELAY_H
#define DELAY_H

#define CLK_CPU 8000000

void Delay_ms(uint32_t arg);
void Delay_us(uint32_t arg);

#endif // DELAY_H
