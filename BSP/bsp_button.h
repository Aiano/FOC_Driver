//
// Created by Aiano on 2022/7/22.
//

#ifndef INC_2022_PROVINCE_A_BSP_BUTTON_H
#define INC_2022_PROVINCE_A_BSP_BUTTON_H

#include "main.h"

#define BUTTON_Port GPIOB
#define BUTTON_LEFT GPIO_PIN_10
#define BUTTON_RIGHT GPIO_PIN_13
#define BUTTON_CONFIRM GPIO_PIN_11
#define BUTTON_CANCEL GPIO_PIN_12

extern uint8_t button_press_pending_flag;

extern uint8_t button_left_press_pending_flag;
extern uint8_t button_right_press_pending_flag;
extern uint8_t button_confirm_press_pending_flag;
extern uint8_t button_cancel_press_pending_flag;

void button_reset_all_flags();

#endif //INC_2022_PROVINCE_A_BSP_BUTTON_H
