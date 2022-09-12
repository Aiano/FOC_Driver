#include "current_sense.h"
#include "adc.h"
#include "FOC.h"
#include "math.h"
#include "FOC_utils.h"
#include "FOC_LPF.h"

float cs_value[3];
float cs_zero_value = 2081;

void cs_init() {
    HAL_ADCEx_Calibration_Start(&hadc1);
}

// 转换函数
// 多通道扫描模式
//
uint8_t cs_get_value() {
    for (int i = 0; i < 3; ++i) {
        // Enables ADC, starts conversion of regular group.
        HAL_ADC_Start(&hadc1);
        // Wait for regular group conversion to be completed.
        HAL_ADC_PollForConversion(&hadc1, 100);
        // 判断ADC是否转换成功
//        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {

        // Get ADC regular group conversion result.
        // Reading register DR automatically clears ADC flag EOC (ADC group regular end of unitary conversion).
        // 该函数读取寄存器DR同时自动清除了EOC(End Of unitary Conversation)标志位

        cs_value[i] = FOC_low_pass_filter(lpf_cs + i, HAL_ADC_GetValue(&hadc1) - cs_zero_value);
//        } else {
//            HAL_ADC_Stop(&hadc1);
        // 如果转换失败就返回-1
//            return -1;
//        }
    }
    // Stop ADC conversion of regular group, disable ADC peripheral.
    HAL_ADC_Stop(&hadc1);

//    cs_value[0] = -cs_value[1] - cs_value[2] + 3 * cs_zero_value;
    return 0;
}




