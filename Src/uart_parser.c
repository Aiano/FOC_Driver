//
// Created by Aiano on 2021/11/25.
//

#include "uart_parser.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define RX_BUFFER_SIZE 256
// receive data byte by byte
uint8_t aRxBuffer = 0;
// and store aRxBuffer into RxBuffer
uint8_t RxBuffer[RX_BUFFER_SIZE];
int RxBufferCounts = 0;
// feedback message
char FeedBackMessage[RX_BUFFER_SIZE + 20];

// callback function list, 20 is an arbitrary number.
UartParserCommandCallback call_list[20];
char call_commands[20];
char *call_labels[20];
int call_count = 0;

void uart_parser_init(UART_HandleTypeDef *huart) {
    // clear rx buffer
    memset(RxBuffer, 0, sizeof(RxBuffer));
    // enable uart interrupt once
    HAL_UART_Receive_IT(huart, (uint8_t *) &aRxBuffer, 1);
}

// redefine Rx Transfer completed callbacks function.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // when enter this function, a byte of data has been stored in aRxBuffer.
    // now, push aRxBuffer into RxBuffer.
    RxBuffer[RxBufferCounts++] = aRxBuffer;
    // if encounter LF, the transfer process has completed.
    if (RxBuffer[RxBufferCounts - 1] == 10) {
        // output feedback message
//        memset(FeedBackMessage, 0, sizeof(FeedBackMessage));
//        strcat(FeedBackMessage, "Received:");
//        strcat(FeedBackMessage, RxBuffer);
//        HAL_UART_Transmit(huart, FeedBackMessage, sizeof(FeedBackMessage), 0xFF);

        // do something to the RxBuffer
        for (int i = 0; i < call_count; i++) {
            // match subscribed command
            if (call_commands[i] == RxBuffer[0]) {
                // enter callback function
                (*call_list[i])(RxBuffer + 1);
            }
        }

        // end of do something to the RxBuffer

        // clear RxBuffer and reset counts
        memset(RxBuffer, 0, sizeof(RxBuffer));
        RxBufferCounts = 0;
    }
    // enable uart interrupt again to receive rest data, or for the next reception
    HAL_UART_Receive_IT(huart, (uint8_t *) &aRxBuffer, 1);
}

void uart_parser_add_command(char command, UartParserCommandCallback callback_func_pointer, char *label) {
    call_list[call_count] = callback_func_pointer;
    call_commands[call_count] = command;
    call_labels[call_count] = label;
    call_count++;
}

void uart_parser_scalar(float *value, char *cmd) {
    if (cmd[0] != 10) {
        *value = atof(cmd);
    }
}

void uart_parser_output(char *output, UART_HandleTypeDef *huart) {
    HAL_UART_Transmit(huart, output, sizeof(output), 0xFF);
}

void uart_parser_output_number(char *description, int number, UART_HandleTypeDef *huart) {
    // output number
    char output_number[30];
    char number_string[10];
    memset(output_number, 0, sizeof(output_number));
    memset(number_string, 0, sizeof(number_string));
    strcat(output_number, description);
    strcat(output_number, ":");
    itoa(number, number_string, 10);
    strcat(output_number, number_string);
    strcat(output_number, "\n");
    HAL_UART_Transmit(huart, output_number, sizeof(output_number), 0xFF);
}