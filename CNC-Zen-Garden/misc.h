#pragma once

#include <stm32f4xx_hal.h>

uint32_t GetMicros();
uint32_t GetMicrosFromISR();
void delayMicroseconds(uint32_t us);
