//
// Created by Aiano on 2021/11/15.
//

#ifndef FOC_DRIVER_ENCODER_H
#define FOC_DRIVER_ENCODER_H

#include <cmath>
#include "adc.h"

#define PI 3.1415

typedef struct {
    ADC_HandleTypeDef *encoderADCHandle;
    int encoder_bit_number;
    float offset_angle;
} FOCEncoderType;

/**
 * @brief conversion function
 * @param adcHandleTypeDef
 * @return adc value
 */
uint16_t get_adc_value(ADC_HandleTypeDef *adcHandleTypeDef);

float get_encoder_angle(FOCEncoderType *encoder);

void set_offset_angle(FOCEncoderType *encoder, float offset);

void set_the_current_angle_to_zero(FOCEncoderType *encoder);
#endif //FOC_DRIVER_ENCODER_H
