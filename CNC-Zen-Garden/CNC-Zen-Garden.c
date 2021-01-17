#include <stm32f4xx_hal.h>
#include <stm32_hal_legacy.h>
#include <stm32f4xx_hal_flash_ex.h>
#include <string.h>

#include "stepper.h"

__attribute__((__section__(".user_data"))) const char userConfig[64];

extern uint32_t _user_data_start;
extern uint32_t _user_data_end;
extern volatile uint32_t UptimeMillis;
UART_HandleTypeDef huart3;

void SystemClock_Config(void);
void USART2_Init(void);

uint32_t Write_Flash(uint32_t address, uint32_t data);

void SysTick_Handler(void)
{
	UptimeMillis++;
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

uint32_t Write_Flash(uint32_t address, uint32_t data)
{
	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	FLASH_Erase_Sector(FLASH_SECTOR_6, VOLTAGE_RANGE_3);
	CLEAR_BIT(FLASH->CR, FLASH_CR_SER);

	char *error;
	uint32_t error_code;

	uint32_t ret = HAL_FLASH_Program(TYPEPROGRAM_WORD, address, data);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);

	switch (ret) {
	case HAL_OK:
		break;
	case HAL_TIMEOUT:
		error = "HAL_FLASH_Program() timeout!";
		break;
	case HAL_ERROR:
		error = "HAL_FLASH_Program() error 0x%08x, see *hal_flash.h for bit definitions\n";
		error_code = HAL_FLASH_GetError();
		break;
	default:
		error = "HAL_FLASH_Program() returned unknown status %lu\n";
		error_code = ret;
	}
	
	HAL_FLASH_Lock();
	
	return error_code;
}

void USART2_Init(void)
{
	__HAL_RCC_USART3_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;

	/* USART configuration */
	HAL_UART_Init(&huart3);
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
	}
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	USART2_Init();

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	//	uint32_t errorcode = Write_Flash((uint32_t)&userConfig[0], 14);
	//
	//	uint8_t ConfArray[16];
	//	uint32_t TestArray[2];
	//
	//	// Copy a part from the userConfig to variable in RAM
	//	for(int i = 0 ; i < 16 ; i++)
	//	{
	//		ConfArray[i] = userConfig[i];
	//	}
	//
	//	// get the address of start and end of user_data in flash
	//	// the & is importand, else you would get the value at the address _user_data_start and _user_data_end points to
	//	TestArray[0] = (uint32_t)&_user_data_start;
	//	TestArray[1] = (uint32_t)&_user_data_end;

	char buff[] = { 'h', 'e', 'l', 'l', 'o', '\r', '\n' };

	stepperMotor_t xMotor = { 0 };
	xMotor.gpioStep.bank = GPIOC;
	xMotor.gpioStep.pin = GPIO_PIN_8;
	xMotor.gpioDir.bank = GPIOC;
	xMotor.gpioDir.pin = GPIO_PIN_6;
	xMotor.stepsPerRev = 200 * 8; // 1.8 deg/s at 1/8
	xMotor.rpm = 300;
	xMotor.direction = 0;

	for (;;)
	{
		HAL_UART_Transmit(&huart3, "hello\r\n", 7, 10);
		
		//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		//		HAL_Delay(500);
		//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		//		HAL_Delay(500);

		xMotor.direction ^= 1;
		stepperStep(&xMotor, 200 * 8);

		HAL_Delay(2000);
	}
}
