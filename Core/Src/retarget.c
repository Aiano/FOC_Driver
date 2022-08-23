#include "main.h"
#include "usart.h"

int __io_putchar(int ch){
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch,1,HAL_MAX_DELAY);
    return ch;
}

int __io_getchar(void){

}