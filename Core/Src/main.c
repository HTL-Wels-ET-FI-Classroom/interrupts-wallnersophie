/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

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

/**
 * @brief draw a random line at coordinates
 * @param x x-coordinate where line starts
 * @param y y-coordinate where line starts
 * @return none
 */
static void drawLine(int x, int y) {
  int x2 = rand() % 240 + 1;
  int y2 = rand() % 320 + 1;
  LCD_DrawLine(x, y, x2, y2);

}

/**
 * @brief draw a random circle at coordinates
 * @param x x-coordinate of circle center
 * @param y y-coordinate of cirlce center
 * @return none
 */
static void drawCircle(int x, int y) {
  int r = rand() % 30 + 1;
  LCD_DrawCircle(x, y, r);
}

/**
 * @brief draw a random ellipsis at coordinates
 * @param x x-coordinate of ellipsis center
 * @param y y-coordinate of ellipsis center
 * @return none
 */
static void drawEllipse(int x, int y) {
  int r1 = rand() % 30 + 1;
  int r2 = rand() % 30 + 1;
  LCD_FillEllipse(x, y, r1, r2);
}

/**
 * @brief draw a random rectangle at coordinates
 * @param x x-coordinate where rectangle starts
 * @param y y-coordinate where rectangle starts
 * @return none
 */
static void drawRectangle(int x, int y) {
  int h = rand() % 50 + 1;
  int w = rand() % 50 + 1;
  LCD_FillRect(x, y, h, w);
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

	/* Initialize all configured peripherals */
	LCD_Init();
	BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());
//	Touchscreen_Calibration();

	/* Clear the LCD */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_ functions
	LCD_DisplayStringAt(0, 0, "HTBLA Wels", CENTER_MODE);
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright xyz", CENTER_MODE);


	/* Infinite loop */
	while (1)
	{
	    // TODO: check if touch has been pressed and draw a random shape
	    // circle, rectangle, triangle, line
		  int x, y;
		  int drawShape = 0;
	    if (GetUserButtonPressed()) {
	      srand(HAL_GetTick());
	      x = rand() % 240 + 1;
	      y = rand() % 320 + 1;
	      drawShape = 1;
	      //LCD_SetPrintPosition(5, 0);
	      //printf("%i, %i", x, y);
	    }
	    if (GetTouchState(&x , &y) == 1) {
	      srand(HAL_GetTick());
	      drawShape = 1;
	    }

	    if (drawShape) {
	      drawShape = 0;
	      int c = rand() | 0xFF000000; // set alpha channel to FF
	      LCD_SetTextColor(c);
	      int s = rand() % 4;
	      switch (s) {
	        case 0:
	          drawLine(x, y);
	          break;
	        case 1:
	          drawCircle(x, y);
	          break;
	        case 2:
	          drawEllipse(x, y);
	          break;
	        case 3:
	          drawRectangle(x, y);
	          break;
	      }
	    }
		// execute main loop every 100ms
//		HAL_Delay(100);
//		int x, y;
//		if (GetTouchState(&x, &y)) {
//			BSP_LCD_FillCircle(x, y, 5);
//		}

	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if rising edge has been detected
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

	BSP_TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
	}

	return touchclick;
}


