/*
 * capsense.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Qinh
 */

#ifndef INC_CAPSENSE_H_
#define INC_CAPSENSE_H_

#include <stdint.h>

typedef union{
	uint8_t data[70];
	struct{
		uint8_t syn[2];
		uint16_t channel_raw[34];
	};
}packet_capsense_t;

extern uint16_t capsense_threshold[34];
extern uint8_t capsense_touch_status[34];
extern uint8_t touch_sheet[34];

void capsense_init();
void capsense_check();

#endif /* INC_CAPSENSE_H_ */
