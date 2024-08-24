#include "serial.h"
#include "stdio.h"
#include "usb_otg.h"

uint8_t rxBuffer0[64] = {0};
uint8_t rxBuffer1[64] = {0};
uint8_t rxBuffer2[64] = {0};

uint8_t rxLen0 = 0;
uint8_t rxLen1 = 0;
uint8_t rxLen2 = 0;


