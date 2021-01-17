#include "misc.h"

volatile uint32_t UptimeMillis;

uint32_t GetMicros()
{
	register uint32_t ms, st;

	do
	{
		ms = UptimeMillis;
		st = SysTick->VAL;
		asm volatile("nop");
		asm volatile("nop");
	} while (ms != UptimeMillis);

	return ms * 1000 - st / ((SysTick->LOAD + 1) / 1000);
}

uint32_t GetMicrosFromISR()
{
	uint32_t st = SysTick->VAL;
	uint32_t pending = SCB->ICSR & SCB_ICSR_PENDSTSET_Msk;
	uint32_t ms = UptimeMillis;

	if (pending == 0)
		ms++;

	return ms * 1000 - st / ((SysTick->LOAD + 1) / 1000);
}

void delayMicroseconds(uint32_t us)
{
	uint32_t startUs = GetMicros();

	while (GetMicros() - startUs < us) ;
}
