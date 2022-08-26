#ifndef FOC_DRIVER_CURRENT_SENSE_H
#define FOC_DRIVER_CURRENT_SENSE_H
#include "main.h"

extern uint16_t cs_value[3];

void cs_init();
uint8_t cs_get_value();
#endif //FOC_DRIVER_CURRENT_SENSE_H
