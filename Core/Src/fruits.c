/*
 * fruits.c
 *
 * SECTION THAT CONTROLS ALL FRUITS AND SLICES
 * POSITION, MOVEMENNT, ACTIONS, AND INTERACTION
 *
 *  Created on: Dec 1, 2023
 *      Author: nicobezz
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "fruits.h"
#include "game_data.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "user_task.h"
#include "game_graphics.h"

/* Private define ------------------------------------------------------------*/
#define LAUNCH_MARGIN 400

/* Private function prototypes -----------------------------------------------*/
void launch_fruit_random(struct Fruit *fruit);
void slice_fruit(struct Fruit *fruit, struct Slice *slice1,
		struct Slice *slice2);
void launch_fruit_physics_update(struct Fruit *fruit);
void slice_physics_update(struct Slice *slice);
uint8_t doesIntersect(double x1, double x2, double y1, double y2,
		double centerX, double centerY);

/* Public variables ----------------------------------------------------------*/
//max five fruits
struct Fruit fruit1, fruit2, fruit3, fruit4, fruit5;

//two slices per fruit
struct Slice slice1_1, slice1_2, slice2_1, slice2_2, slice3_1, slice3_2,
		slice4_1, slice4_2, slice5_1, slice5_2;

/* Functions definition ------------------------------------------------------*/

/**
 * @brief Initializes the launch of max 5 fruits randomly.
 */
void fruit_launch() {
	srand(HAL_GetTick()); //new random seed

	//random number of fruits
	int n = (rand() % 5) + 1;
	vTaskDelay(pdMS_TO_TICKS((rand() % 200) + 300)); //Start wait 200-500ms
	if (state == game) //launch only if the game is running
		launch_fruit_random(&fruit1);
	if (n > 1) {
		vTaskDelay(pdMS_TO_TICKS((rand() % 200))); //0-200 ms
		if (state == game)
			launch_fruit_random(&fruit2);
	}
	if (n > 2) {
		vTaskDelay(pdMS_TO_TICKS((rand() % 200))); //0-200 ms
		if (state == game)
			launch_fruit_random(&fruit3);
	}
	if (n > 3) {
		vTaskDelay(pdMS_TO_TICKS((rand() % 200))); //0-200 ms
		if (state == game)
			launch_fruit_random(&fruit4);
	}
	if (n > 4) { //max 5 new fruits
		vTaskDelay(pdMS_TO_TICKS((rand() % 200))); //0-200 ms
		if (state == game)
			launch_fruit_random(&fruit5);
	}
}

/**
 * @brief Controls all objects flight paths. (physic position update)
 */
void fruit_control() {
	fruit_slice_control();

	if (fruit1.type != off) //if the object is active
		launch_fruit_physics_update(&fruit1); //update position
	if (fruit2.type != off)
		launch_fruit_physics_update(&fruit2);
	if (fruit3.type != off)
		launch_fruit_physics_update(&fruit3);
	if (fruit4.type != off)
		launch_fruit_physics_update(&fruit4);
	if (fruit5.type != off)
		launch_fruit_physics_update(&fruit5);

	if (slice1_1.type != off) //if the object is active
		slice_physics_update(&slice1_1); //update position
	if (slice1_2.type != off)
		slice_physics_update(&slice1_2);
	if (slice2_1.type != off)
		slice_physics_update(&slice2_1);
	if (slice2_2.type != off)
		slice_physics_update(&slice2_2);
	if (slice3_1.type != off)
		slice_physics_update(&slice3_1);
	if (slice3_2.type != off)
		slice_physics_update(&slice3_2);
	if (slice4_1.type != off)
		slice_physics_update(&slice4_1);
	if (slice4_2.type != off)
		slice_physics_update(&slice4_2);
	if (slice5_1.type != off)
		slice_physics_update(&slice5_1);
	if (slice5_2.type != off)
		slice_physics_update(&slice5_2);
}

/**
 * @brief Launches a fruit in a random way.
 * @param pointer to a fruit struct
 */
void launch_fruit_random(struct Fruit *fruit) {
	//x position 600-1000 // screen space is 0-1600
	fruit->x = ((float) rand() / RAND_MAX) * (400) + 600;
	fruit->y = 0;
	fruit->t = 0;
	//random x speed -400 - 400 mm/s
	fruit->speed_x = ((float) rand() / RAND_MAX) * (400 + 400) - 400;
	fruit->speed_y = 1400.0;
	//random fruit type
	fruit->type = (rand() % 5) + 1;
}

/**
 * @brief Calculate the new position of a fruit
 * @param pointer to a fruit struct
 */
void launch_fruit_physics_update(struct Fruit *fruit) {
	const float acc_y = -1500; //mm/s^2 (lower than earths gravity)
	const float t = fruit->t + 0.01; //10ms
	//calculate x shift
	fruit->x = fruit->speed_x * 0.01 + fruit->x; //MRU

	//calcuulate y shift
	fruit->y = 0.5 * acc_y * t * t + fruit->speed_y * t; //MRUA

	//update t
	fruit->t = fruit->t + 0.01;

	//control if "landed"
	if ((int) fruit->y < 0) {
		if (fruit->type != bomb) {
			lives--;
			xTaskNotifyGive(task_display_game_handle); //-1 animation
		}
		fruit->type = off;

	}

}

/**
 * @brief Checks if a fruit has to be sliced.
 */
void fruit_slice_control() {
	// if the last two points are touching the display
	if (touch_points_x[0] != 0 && touch_points_y[0] != 0
			&& touch_points_x[1] != 0 && touch_points_y[1] != 0) {

		//convert screen point to game points
		float x1 = touch_points_x[0] * 2;
		float y1 = (Yresolution - touch_points_y[0]) * 2;
		float x2 = touch_points_x[1] * 2;
		float y2 = (Yresolution - touch_points_y[1]) * 2;
		float x3 = touch_points_x[2] * 2;
		float y3 = (Yresolution - touch_points_y[2]) * 2;

		if (fruit1.type != off) { //if active
			//if the "sword" intersects the fruit
			if (doesIntersect(x1, x2, y1, y2, fruit1.x, fruit1.y))
				//slice the fruit
				slice_fruit(&fruit1, &slice1_1, &slice1_2);
		}
		if (fruit2.type != off) {
			if (doesIntersect(x1, x2, y1, y2, fruit2.x, fruit2.y))
				slice_fruit(&fruit2, &slice2_1, &slice2_2);
		}
		if (fruit3.type != off) {
			if (doesIntersect(x1, x2, y1, y2, fruit3.x, fruit3.y))
				slice_fruit(&fruit3, &slice3_1, &slice3_2);
		}
		if (fruit4.type != off) {
			if (doesIntersect(x1, x2, y1, y2, fruit4.x, fruit4.y))
				slice_fruit(&fruit4, &slice4_1, &slice4_2);
		}
		if (fruit5.type != off) {
			if (doesIntersect(x1, x2, y1, y2, fruit5.x, fruit5.y))
				slice_fruit(&fruit5, &slice5_1, &slice5_2);
		}

	}

}

/**
 * @brief Checks if the sword intercepts a fruit.
 * @param two touch points, fruit coordinate
 * @return 0:NO 1:Yes
 */
uint8_t doesIntersect(double x1, double x2, double y1, double y2,
		double centerX, double centerY) {
	// Calculate the squared distance between the circle center (fruit) and the line (touch sword)
	float d = fabs(
			(y2 - y1) * centerX - (x2 - x1) * centerY + x2 * y1 - y2 * x1)
			/ sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));

	// Check if the distance is less than or equal to the circle radius
	if (d <= FRUIT_RADIUS) {
		// Check if the circle is between the two x points
		if ((centerX + FRUIT_RADIUS >= x1 && centerX - FRUIT_RADIUS <= x2)
				|| (centerX - FRUIT_RADIUS <= x1 && centerX + FRUIT_RADIUS >= x2)) {
			// Check if the circle is between the two y points
			if ((centerY + FRUIT_RADIUS >= y1 && centerY - FRUIT_RADIUS <= y2)
					|| (centerY - FRUIT_RADIUS <= y1
							&& centerY + FRUIT_RADIUS >= y2)) {
				return 1; // Intersection
			}
		}
	}

	return 0; // No intersection
}

/**
 * @brief Slices a fruit.
 * @param pointer to a fruit struct, two slices struct to use
 */
void slice_fruit(struct Fruit *fruit, struct Slice *slice1,
		struct Slice *slice2) {
	//check if it is a bomb
	if (fruit->type == bomb) {
		lives--;
		xTaskNotifyGive(task_display_game_handle); //-1 animation
	}else
		game_points += 10;

	//slice type same as fruit
	slice1->type = fruit->type;
	slice2->type = fruit->type;
	fruit->type = off;

	//same x position
	slice1->x = fruit->x;
	slice2->x = fruit->x;

	//y position has offset
	const float offset = 70;
	slice1->y0 = fruit->y + offset;
	slice2->y0 = fruit->y - offset;

	//random new x speed for each slice
	slice1->speed_x = ((float) rand() / RAND_MAX) * (400 + 400) - 400;
	slice2->speed_x = ((float) rand() / RAND_MAX) * (400 + 400) - 400;

	//reset y speed
	slice1->speed_y = 400.0;
	slice2->speed_y = 400.0;
	slice1->t = 0;
	slice2->t = 0;
}

/**
 * @brief Calculates new position of a slice.
 * @param pointer to  a slice struct
 */
void slice_physics_update(struct Slice *slice) {
	const float acc_y = -1500; //mm/s^2 lower than earths gravity
	const float t = slice->t + 0.01; //10ms
	//calculate x shift
	slice->x = slice->speed_x * 0.01 + slice->x; //MRU

	//calculate y shift
	slice->y = 0.5 * acc_y * t * t + slice->speed_y * t + slice->y0; //MRUA

	//update t
	slice->t = slice->t + 0.01;

	//control if "landed"
	if ((int) slice->y < 0) {
		slice->type = off;
	}
}
