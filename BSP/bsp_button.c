//
// Created by Aiano on 2022/7/22.
//

#include "bsp_button.h"

uint8_t button_press_pending_flag = 0;
uint8_t button_left_press_pending_flag = 0;
uint8_t button_right_press_pending_flag = 0;
uint8_t button_confirm_press_pending_flag = 0;
uint8_t button_cancel_press_pending_flag = 0;

void button_reset_all_flags() {
    button_press_pending_flag = 0;

    button_left_press_pending_flag = 0;
    button_right_press_pending_flag = 0;
    button_confirm_press_pending_flag = 0;
    button_cancel_press_pending_flag = 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    button_press_pending_flag = 1;

    if (HAL_GPIO_ReadPin(BUTTON_Port, GPIO_Pin) == 0) {
        switch (GPIO_Pin) {
            case BUTTON_LEFT:
                button_left_press_pending_flag = 1;
                break;
            case BUTTON_RIGHT:
                button_right_press_pending_flag = 1;
                break;
            case BUTTON_CONFIRM:
                button_confirm_press_pending_flag = 1;
                break;
            case BUTTON_CANCEL:
                button_cancel_press_pending_flag = 1;
                break;
        }
    }
}