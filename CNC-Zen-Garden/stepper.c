#include "stepper.h"
#include "misc.h"

void stepperSetDirection(stepperMotor_t *stepper, uint8_t direction)
{
	stepper->direction = direction;
}

void stepperSetSpeed(stepperMotor_t *stepper, uint32_t rpm)
{
	stepper->rpm = rpm;
}

void delayForRpm(uint32_t stepsPerRev, uint32_t rpm)
{
	delayMicroseconds(60 * 1000 * 1000 / stepsPerRev / rpm);
}

void stepperStep(stepperMotor_t *stepper, int steps)
{
	HAL_GPIO_WritePin(stepper->gpioDir.bank, stepper->gpioDir.pin, stepper->direction);

	while (steps > 0)
	{
		HAL_GPIO_TogglePin(stepper->gpioStep.bank, stepper->gpioStep.pin);
		delayForRpm(stepper->stepsPerRev, stepper->rpm);

		stepper->numberOfSteps++;
		steps--;
	}
}
