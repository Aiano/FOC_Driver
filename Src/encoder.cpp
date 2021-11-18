//
// Created by Aiano on 2021/11/15.
//

#include "encoder.h"

uint16_t get_adc_value(ADC_HandleTypeDef *adcHandleTypeDef) {
    // Enables ADC, starts conversion of regular group.
    HAL_ADC_Start(adcHandleTypeDef);
    // Wait for regular group conversion to be completed.
    HAL_ADC_PollForConversion(adcHandleTypeDef,100);
    // 判断ADC是否转换成功
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(adcHandleTypeDef),HAL_ADC_STATE_REG_EOC)){
        // Get ADC regular group conversion result.
        // Reading register DR automatically clears ADC flag EOC (ADC group regular end of unitary conversion).
        // 该函数读取寄存器DR同时自动清除了EOC(End Of unitary Conversation)标志位
        uint16_t ADC_value = HAL_ADC_GetValue(adcHandleTypeDef);
        // Stop ADC conversion of regular group, disable ADC peripheral.
        HAL_ADC_Stop(adcHandleTypeDef);
        return ADC_value;
    }
    HAL_ADC_Stop(adcHandleTypeDef);
    // 如果转换失败就返回-1
    return -1;
}

float get_encoder_angle(FOCEncoderType *encoder) {
    uint16_t adc_value = get_adc_value(encoder->encoderADCHandle);
    if(adc_value == -1) return -1;

    float output_angle = (float)adc_value / pow(2,encoder->encoder_bit_number) * 2 * PI + encoder->offset_angle;
    return output_angle;
}
