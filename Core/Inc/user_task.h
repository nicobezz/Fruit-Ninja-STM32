/*
 * user_task.h
 *
 *  Created on: Dec 1, 2023
 *      Author: nicobezz
 */

#ifndef INC_USER_TASK_H_
#define INC_USER_TASK_H_

#define TOUCH_CIRCBUFFER_SIZE 10

void freeRTOS_user_init(void);
void tim4_cb();

//public variables, to access touch input points
extern uint16_t touch_points_x[TOUCH_CIRCBUFFER_SIZE];
extern uint16_t touch_points_y[TOUCH_CIRCBUFFER_SIZE];

//FSM state
enum states {
	menu, game, gameover
};
extern enum states state;

//To notify -1 animation
extern TaskHandle_t task_display_game_handle;

#endif /* INC_USER_TASK_H_ */
