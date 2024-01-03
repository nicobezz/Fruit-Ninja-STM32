/*
 * game_graphics.c
 *
 * CONTROLS ALL GRAPOHIC INTERACTIONS WITH THE SCREEN
 *
 *  Created on: Dec 1, 2023
 *      Author: nicobezz
 */

#include <stdint.h>
#include <math.h>
#include "stm32f769i_discovery_lcd.h"	//Include file to use the LDC screen
#include "FreeRTOS.h"
#include "task.h"

//game modules
#include "game_data.h"
#include "user_task.h"
#include "game_graphics.h"
#include "images.h"

/* Private define ------------------------------------------------------------*/
typedef uint32_t ARGB8888;

/* Private function prototypes -----------------------------------------------*/
void Draw_Fruit(struct Fruit *fruit);
void Draw_Slice(struct Slice *slice, int rotation);
void Print_Touch();
void Print_Lives();
void drawBitmap(uint16_t Xpos, uint16_t Ypos, const uint32_t *bitmap,
		int rotation);
uint32_t convertToARGB8888(uint32_t originalPixelValue);

/* Private variables ---------------------------------------------------------*/
static char buffer[20];

/* Functions definition ------------------------------------------------------*/
/**
 * @brief Updates the screen during gameplay.
 * 			It uses Double Buffering update method.
 */
void update_screen_game() {
	static uint8_t layer = LTDC_ACTIVE_LAYER_FOREGROUND;

	//Switches layer between foreground and background
	uint8_t prev_layer = layer;
	if (layer == LTDC_ACTIVE_LAYER_FOREGROUND)
		layer = LTDC_ACTIVE_LAYER_BACKGROUND;
	else
		layer = LTDC_ACTIVE_LAYER_FOREGROUND;
	//select the new layer
	BSP_LCD_SelectLayer(layer);

	//Draw background image
	BSP_LCD_DrawBitmap(0, 0, bg_bmp);

	//SLICE Graphic
	if (slice1_1.type != off) //if slice active
		Draw_Slice(&slice1_1, 0); //draw the slice
	if (slice1_2.type != off)
		Draw_Slice(&slice1_2, 180); //180 degrees rotated
	if (slice2_1.type != off)
		Draw_Slice(&slice2_1, 0);
	if (slice2_2.type != off)
		Draw_Slice(&slice2_2, 180);
	if (slice3_1.type != off)
		Draw_Slice(&slice3_1, 0);
	if (slice3_2.type != off)
		Draw_Slice(&slice3_2, 180);
	if (slice4_1.type != off)
		Draw_Slice(&slice4_1, 0);
	if (slice4_2.type != off)
		Draw_Slice(&slice4_2, 180);
	if (slice5_1.type != off)
		Draw_Slice(&slice5_1, 0);
	if (slice5_2.type != off)
		Draw_Slice(&slice5_2, 180);

	//FRUIT Graphic
	if (fruit1.type != off) //if fruit is active
		Draw_Fruit(&fruit1); //draw the fruit
	if (fruit2.type != off)
		Draw_Fruit(&fruit2);
	if (fruit3.type != off)
		Draw_Fruit(&fruit3);
	if (fruit4.type != off)
		Draw_Fruit(&fruit4);
	if (fruit5.type != off)
		Draw_Fruit(&fruit5);

	//Print the sword
	Print_Touch();
	//print points a lives
	Print_Lives();

	//Wait DMA finishes trasfering the new layer data to the display RAM
	while (HAL_DMA2D_GetState(&hdma2d_discovery) != HAL_DMA2D_STATE_READY)
		vTaskDelay(pdMS_TO_TICKS(1));

	//Critical section -> old-new layer switch
	taskENTER_CRITICAL();
	BSP_LCD_SetLayerVisible(prev_layer, DISABLE); //disable the old layer
	BSP_LCD_SetLayerVisible(layer, ENABLE); //enables the new layer
	taskEXIT_CRITICAL();
}

/**
 * @brief Updates the screen with the menu;
 *
 */
void update_screen_menu() {
	BSP_LCD_DrawBitmap(0, 0, bg_bmp);
	BSP_LCD_DrawBitmap(240, 70, start_bmp);
	//BSP_LCD_DisplayStringAt(0, 210, "PRESS THE SCREEN", CENTER_MODE);
}

/**
 * @brief Updates the screen with the game over;
 *
 */
void update_screen_gameover() {
	//BSP_LCD_DrawBitmap(0, 0, bg_bmp);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, 210, "GAME OVER", CENTER_MODE);
	sprintf(buffer, "Points: %d", game_points);
	BSP_LCD_DisplayStringAt(0, 0, buffer, LEFT_MODE);
}

/**
 * @brief Draws a fruit on the screen.
 * @param Pointer to the fruit to draw.
 */
void Draw_Fruit(struct Fruit *fruit) {
	//convert game coordinates to pixel
	uint32_t x = fruit->x / 2.0;
	uint32_t y = Yresolution - (fruit->y / 2.0);

	//draw the correct image corresponding to the fruit type
	switch (fruit->type) {
	case bomb:
		drawBitmap(x - 50, y - 50, (uint32_t*) bomb_bmp, 180);
		break;
	case apple:
		drawBitmap(x - 50, y - 50, (uint32_t*) apple_bmp, 180);
		break;
	case coconut:
		drawBitmap(x - 50, y - 50, (uint32_t*) coconut_bmp, 180);
		break;
	case watermelon:
		drawBitmap(x - 50, y - 50, (uint32_t*) watermelon_bmp, 180);
		break;
	case orange:
		drawBitmap(x - 50, y - 50, (uint32_t*) orange_bmp, 180);
		break;
	}
}

/**
 * @brief Draws the sword on the screen.
 *
 */
void Print_Touch() {
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	//cycle the touch points buffer
	for (uint16_t i = 0; i < TOUCH_CIRCBUFFER_SIZE - 1; i++) {
		if (touch_points_x[i] != 0 && touch_points_y[i] != 0
				&& touch_points_x[i + 1] != 0 && touch_points_y[i + 1] != 0) {
			//if there are two points, draw a line connecting them
			BSP_LCD_DrawLine(touch_points_x[i], touch_points_y[i],
					touch_points_x[i + 1], touch_points_y[i + 1]);
		}

	}
}

/**
 * @brief Prints score and lives.
 *
 */
void Print_Lives() {
	static uint32_t time = 0;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	//Print X corresponding to the lives
	if (lives == 3)
		BSP_LCD_DisplayStringAt(0, 0, "XXX", RIGHT_MODE);
	else if (lives == 2)
		BSP_LCD_DisplayStringAt(0, 0, "XX", RIGHT_MODE);
	else if (lives == 1)
		BSP_LCD_DisplayStringAt(0, 0, "X", RIGHT_MODE);
	else if (lives == 0)
		BSP_LCD_DisplayStringAt(0, 210, "GAME OVER", CENTER_MODE);

	//Print the game points
	sprintf(buffer, "Points: %d", game_points);
	BSP_LCD_DisplayStringAt(0, 0, buffer, LEFT_MODE);

	//-1 animation
	//get if there was a live loss
	uint32_t retval = ulTaskNotifyTake(pdTRUE, 0);
	if (retval > 0)
		time = HAL_GetTick(); //reset time
	//if there was a live loss in the last second
	if ((HAL_GetTick() - time) < pdMS_TO_TICKS(1000)) {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		//display a "-1" going down corresponding to the time
		BSP_LCD_DisplayStringAt(0, 10 + (HAL_GetTick() - time) / 25, "-1",
				RIGHT_MODE);
	}
}

/**
 * @brief Draws a fruit slice on the screen.
 * @param Pointer to the slice to draw, rotation of the slice.
 */
void Draw_Slice(struct Slice *slice, int rotation) {
	//convert game coordinates to pixel
	uint32_t x = slice->x / 2.0;
	uint32_t y = Yresolution - (slice->y / 2.0);

	//print correct slice corresponding to the type
	switch (slice->type) {
	case apple:
		drawBitmap(x - 50, y - 50, (uint32_t*) sliced_apple_bmp, rotation);
		break;
	case coconut:
		drawBitmap(x - 50, y - 50, (uint32_t*) sliced_coconut_bmp, rotation);
		break;
	case watermelon:
		drawBitmap(x - 50, y - 50, (uint32_t*) sliced_watermelon_bmp, rotation);
		break;
	case orange:
		drawBitmap(x - 50, y - 50, (uint32_t*) sliced_orange_bmp, rotation);
		break;
	}
}

/**
 * @brief User function to draw a 100x100px bitmap with transparency and rotation.
 * @param Image coordinates, pointer to bitmap, image rotation
 */
void drawBitmap(uint16_t Xpos, uint16_t Ypos, const uint32_t *bitmap,
		int rotation) {
	uint16_t x, y;

	//cycle trough height and width of the image
	for (y = 0; y < 100; y++) {
		for (x = 0; x < 100; x++) {
			uint16_t rotatedX, rotatedY;

			//convert coordinates corresponding to the rotation
			if (rotation == 180) {
				rotatedX = 99 - x;
				rotatedY = 99 - y;
			} else {
				rotatedX = x;
				rotatedY = y;
			}
			//get the pixel value
			uint32_t pixelValue = bitmap[(rotatedY * 50 + rotatedX / 2) + 13];

			//if it is not black (trasparency)
			if ((pixelValue & 0xFF000000) != 0) {
				//convert pixel value to ARGB8888
				pixelValue = convertToARGB8888(pixelValue);
				//if the coordinate is on the screen
				if (Xpos + x > 0&& Ypos + y > 0 && Xpos + x < Xresolution
				&& Ypos + y < Yresolution)
					BSP_LCD_DrawPixel(Xpos + x, Ypos + y, pixelValue);
					//Draw the pixel
			}
		}
	}
}

/**
 * @brief User function to convert RGB565 to ARGB8888.
 * @param RGB565 Pixel value
 * @return ARGB8888 Pixel value
 */
uint32_t convertToARGB8888(uint32_t originalPixelValue) {
	// Extract RGB components from RGB565
	uint8_t red = ((originalPixelValue >> 8) & 0xF8)
			| ((originalPixelValue >> 13) & 0x07);
	uint8_t green = ((originalPixelValue >> 3) & 0xFC)
			| ((originalPixelValue >> 9) & 0x03);
	uint8_t blue = ((originalPixelValue << 3) & 0xF8)
			| ((originalPixelValue >> 2) & 0x07);

	// Alpha value (assuming fully opaque)
	uint8_t alpha = 0xFF;

	// Pack into ARGB8888 format
	uint32_t argb8888Value = (alpha << 24) | (red << 16) | (green << 8) | blue;

	return argb8888Value;
}
