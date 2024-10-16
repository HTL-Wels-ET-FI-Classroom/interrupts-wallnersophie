/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static volatile int cntInterrupt;
static volatile int cntColourSwitch;
/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

void EXTI0_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	cntInterrupt ++;
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
}
void EXTI2_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	cntColourSwitch ++;
}


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_SetPrintPosition(3, 0);
	printf("EXTI Interrupt");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "Sophie Wallner", CENTER_MODE);


	GPIO_InitTypeDef pa0;
	GPIO_InitTypeDef pg2;
	GPIO_InitTypeDef pg13;


	pa0.Mode = GPIO_MODE_IT_RISING;
	pa0.Alternate = 0;
	pa0.Pin = GPIO_PIN_0;
	pa0.Pull = GPIO_NOPULL;
	pa0.Speed = GPIO_SPEED_FAST;

	pg2.Mode = GPIO_MODE_IT_RISING;
	pg2.Alternate = 0;
	pg2.Pin = GPIO_PIN_2;
	pg2.Pull = GPIO_NOPULL;
	pg2.Speed = GPIO_SPEED_FAST;

	pg13.Mode = GPIO_MODE_OUTPUT_PP;
	pg13.Alternate = 0;
	pg13.Pin = GPIO_PIN_13;
	pg13.Pull = GPIO_NOPULL;
	pg13.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &pa0);
	HAL_GPIO_Init(GPIOG, &pg2);
	HAL_GPIO_Init(GPIOG, &pg13);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);



	int cnt1 = 0;
	int cnt2 = 0;
	/* Infinite loop */
	while (1)
	{
		//execute main loop every 100ms
		HAL_Delay(1);


		// Abfrage welcher Timer laufen soll und welcher cnt hochgezählt wird.
		if(cntInterrupt % 2 == 0){
			cnt1++;
		}

		if(cntInterrupt % 2 == 1){
			cnt2++;
		}

		// Abfrage welche Farbe und setzen der Farbe
		if(cntColourSwitch % 2 == 0){
			LCD_SetTextColor(LCD_COLOR_BLUE);
		}
		if(cntColourSwitch % 2 == 1){
			LCD_SetTextColor(LCD_COLOR_GREEN);
		}


		// Ausgabe Timer
		LCD_SetFont(&Font20);
		LCD_SetPrintPosition(5, 0);
		printf("   Timer: %.1f", cnt1/100.0);

		LCD_SetPrintPosition(7, 0);
		printf("   Timer: %.1f", cnt2/100.0);


	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


