#include "vl53l0x.h"
#include <math.h>
//#include "PersonalMath.h"

struct VL53L0xData VL53L0x_Bottom;        //底部连接在IIC2的VL53L0x的数据


void VL53L0x_init(I2C_HandleTypeDef *hi2c) {
    uint8_t VL53L0x_SendData[2] = {0x01};
    uint8_t VL53L0x_RecData[5];

    /*	//Revision ID:
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_IDENTIFICATION_REVISION_ID, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData, 1, 10);
        //Device ID:
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_IDENTIFICATION_MODEL_ID, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData+1, 1, 10);
        //PRE_RANGE_CONFIG_VCSEL_PERIOD =
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData+2, 1, 10);
        //FINAL_RANGE_CONFIG_VCSEL_PERIOD=
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData+3, 1, 10);
        */
    HAL_I2C_Mem_Write(hi2c, VL53L0x_add, VL53L0X_REG_SYSRANGE_START, I2C_MEMADD_SIZE_8BIT, VL53L0x_SendData, 1, 1);
    //HAL_Delay(500);

    VL53L0x_SendData[1] = 100;
    while (VL53L0x_SendData[1]--) {
        HAL_Delay(1);
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_RESULT_RANGE_STATUS, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData + 4,
                         1, 1);
        if (VL53L0x_RecData[4] & 0x01) break;
    }
}

//检查VL53L0x是否正常
//检查成功返回0
//检查失败返回1
uint8_t VL53L0x_Check(I2C_HandleTypeDef *hi2c) {
    uint8_t VL53L0x_SendData[2] = {0x01};
    uint8_t VL53L0x_RecData[5];

    //Revision ID:
    HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_IDENTIFICATION_REVISION_ID, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData,
                     1, 1);
    //Device ID:
    HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_IDENTIFICATION_MODEL_ID, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData + 1,
                     1, 1);
    //PRE_RANGE_CONFIG_VCSEL_PERIOD =
    HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD, I2C_MEMADD_SIZE_8BIT,
                     VL53L0x_RecData + 2, 1, 1);
    //FINAL_RANGE_CONFIG_VCSEL_PERIOD=
    HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD, I2C_MEMADD_SIZE_8BIT,
                     VL53L0x_RecData + 3, 1, 1);

    HAL_I2C_Mem_Write(hi2c, VL53L0x_add, VL53L0X_REG_SYSRANGE_START, I2C_MEMADD_SIZE_8BIT, VL53L0x_SendData, 1, 1);
    HAL_Delay(500);

    VL53L0x_SendData[1] = 100;
    while (VL53L0x_SendData[1]--) {
        HAL_Delay(10);
        HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_RESULT_RANGE_STATUS, I2C_MEMADD_SIZE_8BIT, VL53L0x_RecData + 4,
                         1, 1);
        if (VL53L0x_RecData[4] & 0x01) break;
    }

    if (VL53L0x_RecData[4] & 0x01)
        return 0;                //检查成功返回0
    else return 1;    //检查失败返回1
}

//VL53L0x转换一次
void VL53L0x_StartConversion(I2C_HandleTypeDef *hi2c) {
    uint8_t VL53L0x_SendData[1] = {0x01};
    HAL_I2C_Mem_Write(hi2c, VL53L0x_add, VL53L0X_REG_SYSRANGE_START, I2C_MEMADD_SIZE_8BIT, VL53L0x_SendData, 1, 1);
}

uint16_t makeuint16(int lsb, int msb) {
    return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}


//VL53L0x读取距离等数据反馈信息
void VL53L0x_ReadDistance(I2C_HandleTypeDef *hi2c, struct VL53L0xData *VL_temp) {
    //记录历史有效值
    VL_temp->dist_last = VL_temp->distValid;

    HAL_I2C_Mem_Read(hi2c, VL53L0x_add, VL53L0X_REG_RESULT_RANGE_STATUS, I2C_MEMADD_SIZE_8BIT, VL_temp->vtemp, 12, 1);

    VL_temp->acnt = makeuint16(VL_temp->vtemp[7], VL_temp->vtemp[6]);
    VL_temp->scnt = makeuint16(VL_temp->vtemp[9], VL_temp->vtemp[8]);
    VL_temp->dist = makeuint16(VL_temp->vtemp[11], VL_temp->vtemp[10]);
    VL_temp->DeviceRangeStatusInternal = ((VL_temp->vtemp[0] & 0x78) >> 3);


    //提取有效值
    if (VL_temp->dist <= 0x0014)        //距离数据无效
        VL_temp->distValid = VL_temp->dist_last;
    else    //有效
        VL_temp->distValid = VL_temp->dist;
}