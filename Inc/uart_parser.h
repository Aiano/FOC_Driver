//
// Created by Aiano on 2021/11/25.
//

#ifndef HAL_UART_PARSER_UART_PARSER_H
#define HAL_UART_PARSER_UART_PARSER_H

#include "usart.h"

#ifdef __cplusplus
extern "C"{
#endif

    typedef void (* UartParserCommandCallback)(char *);

    void uart_parser_init(UART_HandleTypeDef *huart);
    void uart_parser_add_command(char command, UartParserCommandCallback callback_func_pointer, char *label);
    void uart_parser_scalar(float* value, char *cmd);
    void uart_parser_output(char *output, UART_HandleTypeDef *huart);
    void uart_parser_output_number(char* description,int number, UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif //HAL_UART_PARSER_UART_PARSER_H
