#ifndef __VL53L0X_H
#define __VL53L0X_H
#include "stm32f1xx_hal.h"
#include "i2c.h"
#ifdef __cplusplus
extern "C"{
#endif
//
//#include "Structure.h"
//#include "Algorithm_filter.h"


#define VL_CS1_Enable 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,(GPIO_PinState)1);	//左侧VL53L0X的PB2拉高使能
#define VL_CS1_Disable 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,(GPIO_PinState)0);	//左侧VL53L0X的PB2拉低失能

#define VL_CS2_Enable 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,(GPIO_PinState)1);	//右侧VL53L0X的PB1拉高使能
#define VL_CS2_Disable 		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,(GPIO_PinState)0);	//右侧VL53L0X的PB1拉低失能


//寄存器操作函数
#define VL53L0x_add 		0x52			//VL53L0x的IIC器件地址

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID         0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID      0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START                  0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS             0x14


struct VL53L0xData {
    uint8_t vtemp[12];
    uint16_t acnt;		//环境统计,激光光强
    uint16_t scnt;		//信号数
    uint16_t dist;		//距离，单位mm	最原始数据
    uint8_t DeviceRangeStatusInternal;

    uint16_t dist_last;		//历史有效值，用来判断

    uint16_t distValid;		//原始距离值提取后的有效值
    uint16_t dist_buff[15];		//测距的滑窗缓存
    uint16_t distValidFinal;

    float GroundDis;
    float GroundDis_last;

    float Speed;
    float Speed_last;

    uint16_t distFilted;	//卡尔曼滤波后的距离值
    uint16_t distFilted_1;//卡尔曼后 + 一阶低通滤波

    uint8_t Flag_OverRange;	//超出量程标志位

    /*	uint16_t dist_Original;	//原始距离值
        uint16_t dist_OriginalValid;	//原始距离值提取后的有效值
        uint16_t dist_OriginalValid_lsat;*/
};

extern struct VL53L0xData  VL53L0x_Bottom;		//底部连接在IIC2的VL53L0x的数据



void VL53L0x_init(I2C_HandleTypeDef *hi2c);

//检查VL53L0x是否正常
uint8_t VL53L0x_Check(I2C_HandleTypeDef *hi2c);

//VL53L0x转换一次
void VL53L0x_StartConversion(I2C_HandleTypeDef *hi2c);

//VL53L0x读取距离等数据反馈信息
void VL53L0x_ReadDistance(I2C_HandleTypeDef *hi2c, struct VL53L0xData *VL_temp);

void VL53L0x_DataFitting(void);

#ifdef __cplusplus
}
#endif

#endif