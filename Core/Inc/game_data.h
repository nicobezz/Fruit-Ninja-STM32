/*
 * game_data.h
 *
 * COINTAINS ALL GAME DATA
 *
 *  Created on: Dec 1, 2023
 *      Author: nicobezz
 */

#ifndef INC_GAME_DATA_H_
#define INC_GAME_DATA_H_

#include <stdint.h>

//game physics variables
#define GAME_HEIGHT 944 //0.944m
#define GAME_WIDTH 1600 //1.6m
#define FRUIT_RADIUS 50 //touch sensitivity

enum FruitType {
	off, bomb, apple, coconut, watermelon, orange
};

struct Fruit {
	float x; 			//mm
	float y; 			//mm
	float speed_x; 		//mm/s
	float speed_y; 		//mm/s
	float t;			//ms
	enum FruitType type;
};

struct Slice {
	float x; 			//mm
	float y; 			//mm
	float y0; 			//mm
	float speed_x; 		//mm/s
	float speed_y; 		//mm/s
	float t;			//ms
	enum FruitType type;
};

extern struct Fruit fruit1, fruit2, fruit3, fruit4, fruit5;
extern struct Slice slice1_1, slice1_2, slice2_1, slice2_2, slice3_1, slice3_2,
		slice4_1, slice4_2, slice5_1, slice5_2;

extern uint16_t game_points;
extern uint8_t lives;


#endif /* INC_GAME_DATA_H_ */
