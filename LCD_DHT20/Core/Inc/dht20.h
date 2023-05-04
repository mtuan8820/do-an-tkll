/*
 * dht20.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Hello
 */

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdio.h>
#include "main.h"
//#include "lcd16x2.h"
//#include "software_timer.h"

#define INIT 0
#define READ 1

extern uint16_t value_x10[2];
extern char temp[20],humid[20];

void dht20_init(void); //init module

void dht20_reset(uint8_t);//reset module

void dht20_read(uint16_t*);//read sensor

void dht20_start(void);//start sensor

void init_reading(void);

//void reading_fsm_run(void);

#endif /* INC_DHT20_H_ */
