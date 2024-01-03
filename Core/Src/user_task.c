/*
 * user_task.c
 *
 * CONTAINS ALL USER TASKS
 *
 *  Created on: Dec 1, 2023
 *      Author: nicobezz
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f769i_discovery_lcd.h"	//Include file to use the LDC screen
#include "stm32f769i_discovery_ts.h"	//Include file to use the touchscreen
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "user_task.h"
#include "tim.h"
#include "timers.h"

//game modules
#include "game_data.h"
#include "fruits.h"
#include "game_graphics.h"

/* Private define ------------------------------------------------------------*/

/* Private data types definition ---------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
uint16_t touch_points_x[TOUCH_CIRCBUFFER_SIZE] = { 0 };
uint16_t touch_points_y[TOUCH_CIRCBUFFER_SIZE] = { 0 };

uint16_t game_points;
uint8_t lives;

enum states state = menu; //FSM State

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_FSM_handle;				//fsm Task Handle
static TaskHandle_t task_fruit_launch_handle;		//fruit launch Task Handle
static TaskHandle_t task_fruit_control_handle;		//fruit physic Task Handle
static TaskHandle_t task_touch_handle;				//Touchscreen Task Handle
TaskHandle_t task_display_game_handle;		//display Task Handle

static TimerHandle_t sw_tim_touch_handle; 			//sw timer -> touch task
static TimerHandle_t sw_tim_disp_handle; 			//sw timer -> dislpay task

SemaphoreHandle_t sem_fruit_control;
SemaphoreHandle_t sem_touch;
SemaphoreHandle_t sem_disp;

/* Private function prototypes -----------------------------------------------*/
static void task_FSM(void *pvParameters);
static void task_fruit_launch(void *pvParameters);
static void task_fruit_control(void *pvParameters);
static void task_display_game(void *pvParameters);
static void task_touch_fct(void *pvParameters);
static void sw_tim_touch_cb();
static void sw_tim_disp_cb();

/* Functions definition ------------------------------------------------------*/
/**
 * @brief Create the FreeRTOS objects and tasks.
 * @return true if the tasks are created, false otherwise.
 */

void freeRTOS_user_init(void) {
	bool retval = true;

	sem_fruit_control = xSemaphoreCreateBinary();
	if (sem_fruit_control == NULL)
		retval = false;

	sem_touch = xSemaphoreCreateBinary();
	if (sem_touch == NULL)
		retval = false;

	sem_disp = xSemaphoreCreateBinary();
	if (sem_touch == NULL)
		retval = false;

	retval &= xTaskCreate(task_FSM,		//Task function
			"Fruit Launch",					//Task function comment
			256,							//Task stack dimension (1kB)
			NULL,							//Task parameter
			2,								//Task priority
			&task_FSM_handle);			//Task handle

	retval &= xTaskCreate(task_fruit_launch,		//Task function
			"Fruit Launch",					//Task function comment
			256,							//Task stack dimension (1kB)
			NULL,							//Task parameter
			1,								//Task priority
			&task_fruit_launch_handle);			//Task handle

	retval &= xTaskCreate(task_fruit_control,		//Task function
			"Fruit Control",					//Task function comment
			256,							//Task stack dimension (1kB)
			NULL,							//Task parameter
			2,								//Task priority
			&task_fruit_control_handle);			//Task handle

	retval &= xTaskCreate(task_display_game,		//Task function
			"Display game",					//Task function comment
			256,							//Task stack dimension (1kB)
			NULL,							//Task parameter
			2,								//Task priority
			&task_display_game_handle);			//Task handle

	retval &= xTaskCreate(task_touch_fct,		//Task function
			"Touch control",					//Task function comment
			256,							//Task stack dimension (1kB)
			NULL,							//Task parameter
			3,								//Task priority
			&task_touch_handle);			//Task handle

	sw_tim_touch_handle = xTimerCreate("Timer touch", //Timer description
			pdMS_TO_TICKS(20), //Timer period
			pdTRUE, //Timer is periodic
			NULL, //Timer ID
			sw_tim_touch_cb); //Timer callback fct.

	sw_tim_disp_handle = xTimerCreate("Timer display", //Timer description
			pdMS_TO_TICKS(33), //Timer period
			pdTRUE, //Timer is periodic
			NULL, //Timer ID
			sw_tim_disp_cb); //Timer callback fct.
}
/**
 * @brief FSM task, controls, start and game over.
 */
static void task_FSM(void *pvParameters) {
	while (1) {
		switch (state) {
		case menu:
			//display the menu graphic
			update_screen_menu();
			//wait screen touched
			while (touch_points_x[0] == 0 && touch_points_y[0] == 0)
				;

			//if "play" is touched
			if (touch_points_x[0] > 290 && touch_points_x[0] < 490
					&& touch_points_y[0] > 295 && touch_points_y[0] < 360) {
				lives = 3; //reset lives
				game_points = 0; //reset game points
				//start the display game task
				xTimerStart(sw_tim_disp_handle, portMAX_DELAY);
				state = game;
			}

			//wait screen released
			while (touch_points_x[0] != 0 && touch_points_y[0] != 0)
				;
			break;

		case game:
			//if there are no lives left
			if (lives <= 0) {
				state = gameover;
				//stop the display game task
				xTimerStop(sw_tim_disp_handle, portMAX_DELAY);

				//reset all objects
				fruit1.type = off;
				fruit2.type = off;
				fruit3.type = off;
				fruit4.type = off;
				fruit5.type = off;
				slice1_1.type = off;
				slice1_2.type = off;
				slice2_1.type = off;
				slice2_2.type = off;
				slice3_1.type = off;
				slice3_2.type = off;
				slice4_1.type = off;
				slice4_2.type = off;
				slice5_1.type = off;
				slice5_2.type = off;

				//wait screen released
				while (touch_points_x[0] != 0 && touch_points_y[0] != 0)
					;
			}

			vTaskDelay(pdMS_TO_TICKS(33)); //max 30Hz
			break;

		case gameover:
			//display game over
			update_screen_gameover();

			//wait screen touched
			while (touch_points_x[0] == 0 && touch_points_y[0] == 0)
				;

			state = menu;

			//wait screen released
			while (touch_points_x[0] != 0 && touch_points_y[0] != 0)
				;
			break;

		default:
			state = menu;
		}
	}
}

/**
 * @brief Launches fruits when necessary.
 */
static void task_fruit_launch(void *pvParameters) {
	while (1) {
		//if all fruits are off
		if (state == game && fruit1.type == off && fruit2.type == off
				&& fruit3.type == off && fruit4.type == off
				&& fruit5.type == off)
			fruit_launch(); //new random launch sequence
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/**
 * @brief Controls all objects (fruits&slices) actions, position, ...
 */
static void task_fruit_control(void *pvParameters) {
	HAL_TIM_Base_Start_IT(&htim4);
	BaseType_t retval;
	while (1) {
		//Wait for the semaphore indefinitely
		retval = xSemaphoreTake(sem_fruit_control, portMAX_DELAY);
		if (retval == pdTRUE) {
			fruit_control();
		}

	}
}

/**
 * @brief Displays the game objects.
 */
static void task_display_game(void *pvParameters) {
	BaseType_t retval;

	while (1) {
		//Wait for the semaphore indefinitely
		retval = xSemaphoreTake(sem_disp, portMAX_DELAY);
		if (retval == pdTRUE) {
			//max 30Hz
			update_screen_game();
		}
	}
}

/**
 * @brief Controls user touch inputs.
 */
static void task_touch_fct(void *pvParameters) {
	TS_StateTypeDef TS_State;
	BaseType_t retval;
	xTimerStart(sw_tim_touch_handle, portMAX_DELAY);
	while (1) {
		retval = xSemaphoreTake(sem_touch, portMAX_DELAY);

		if (retval == pdTRUE) {
			//50Hz

			BSP_TS_GetState(&TS_State);
			//shifts touch points buffer to make place for new value
			for (int i = TOUCH_CIRCBUFFER_SIZE - 1; i > 0; i--) {
				touch_points_x[i] = touch_points_x[i - 1];
				touch_points_y[i] = touch_points_y[i - 1];
			}

			//If at least one finger is on the touch
			if (TS_State.touchDetected) {
				//save the new point in the array
				touch_points_x[0] = TS_State.touchX[0];
				touch_points_y[0] = TS_State.touchY[0];

			} else {
				//interpreted as no touch
				touch_points_x[0] = 0;
				touch_points_y[0] = 0;
			}
		}
	}
}

/**
 * @brief TIM4 callback.
 */
void tim4_cb() {
	//10ms - 100Hz
	//Unlocks fruit control task
	xSemaphoreGiveFromISR(sem_fruit_control, NULL);
}
static void sw_tim_touch_cb() {
	//20ms - 50Hz
	//Unlocks touch task
	xSemaphoreGiveFromISR(sem_touch, NULL);
}

static void sw_tim_disp_cb() {
	//33ms - 30Hz
	//Unlocks Display game task
	xSemaphoreGiveFromISR(sem_disp, NULL);
}
