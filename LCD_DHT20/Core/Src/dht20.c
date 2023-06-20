/*
 * dht20.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Hello
 */


#include "dht20.h"

extern I2C_HandleTypeDef hi2c1;

#define SLAVE_ADDRESS_DHT20 (0xB8)

//bien luu tru gia tri temp va humid
double temp, humid;

void dht20_init(void){
	//Set register when call a wrong reset
	uint8_t init[3];

	init[0] = 0xA8;
	init[1] = 0x00;
	init[2] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) init, 3, 0xFF);
	HAL_Delay(20);

	init[0] = 0xBE;
	init[1] = 0x08;
	init[2] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) init, 3, 0xFF);
	HAL_Delay(20);
}

void dht20_reset(uint8_t regist){
	//reset register
	uint8_t reset[3], reply[3];

	reset[0] = regist;
	reset[1] = 0x00;
	reset[2] = 0x00;
 	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) reset, 3, 0xFF);
	HAL_Delay(10);

	HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20 | 0x01, (uint8_t*) reply, 3, 0xFF);
	reset[0] = 0xB0 | regist;
	reset[1] = reply[1];
	reset[2] = reply[2];
	HAL_Delay(10);

	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) reset, 3, 0xFF);
}

void dht20_start(void){
	//query the DHT20
	uint8_t status[1];
	HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20 | 0x01, (uint8_t*) status, 1, 0xFF);

	if((status[0] & 0x18) != 0x18){
		dht20_reset(0x1B);
		dht20_reset(0x1C);
		dht20_reset(0x1E);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
	}

	if ((status[0] & 0x18) == 0x18){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
	}

	uint8_t data[3] = {0xAC, 0x33, 0x00};

	//HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) trigger, 1, 0xFF);
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) data, 3, 0xFF);
	HAL_Delay(80);
}

void dht20_read(){
	dht20_start();
	uint8_t data[7];
//	uint32_t Temper = 0, Humid = 0;
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_DHT20, 0x00, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADDRESS_DHT20, (uint8_t*) data, 5, 1000);

	if (data[0] + data[1] + data[2] + data[3] != data[4]) {
		temp = -1;
		humid = -1;
	}
	else {
		humid = data[0] + (double)data[1]/(double)10;
		temp = data[2] + (double)data[3]/(double)10;
	}

}

//void init_reading(void){
//	dht20_init();
//	dht20_read(value_x10);
//}

//void reading_fsm_run(void){
//	switch(status){
//		case INIT:
//			setTimer1(1);
//			status = READ;
//			break;
//		case READ:
//			if (timer1_flag == 1){
//				dht20_read(value_x10);
//				//11011111 is degree character (manual)
//				sprintf(temp, "Temp:  %d.%d %cC",value_x10[1]/10,value_x10[1]%10 ,0b11011111);
//				sprintf(humid,"Humid: %01d.%d %%   ",value_x10[0]/10,value_x10[0]%10);
//				setTimer1(300);
//			}
//			break;
//		default:
//			break;
//	}
//	lcd_show_value();
//}
