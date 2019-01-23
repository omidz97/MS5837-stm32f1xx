// MS5837 module library - version:1.0.0
#ifndef MS5837_H
#define MS5837_H

#include <stdint.h>
#include "stm32f1xx_hal.h"


 
#define Address 0xEC
#define RESET_AD 0x1E



void prom(void);
void convert(void);

#endif
