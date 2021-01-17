#pragma once

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>

typedef struct
{
	GPIO_TypeDef *bank;
	uint16_t pin;
} GPIOPin;

typedef struct
{
	GPIOPin gpioStep;
	GPIOPin gpioDir;

	GPIO_PinState pinState;

	uint8_t direction;
	uint32_t rpm;
	uint32_t stepsPerRev;
	uint32_t numberOfSteps;
} stepperMotor_t;

void stepperSetDirection(stepperMotor_t *stepper, uint8_t direction);
void stepperSetSpeed(stepperMotor_t *stepper, uint32_t rpm);
void stepperStep(stepperMotor_t *stepper, int steps);
