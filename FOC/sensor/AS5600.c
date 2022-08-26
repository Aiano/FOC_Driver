#include "AS5600.h"
#include "i2c.h"

void AS5600_init() {
    uint8_t data = 0x03;
    HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, _conf_hi, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);

    return;
}

/**************************************************************
 * 函数名：uint8_t AS5600_readOneByte(uint8_t in_adr)
 * 函数功能：给定地址读一个字节
 * 输入参数：in_adr 待读数据的地址
 * 输出参数：retVal读出的数据
 * 返回值：无
**************************************************************/
uint8_t AS5600_readOneByte(uint8_t in_adr)//读一个字节函数，从5600的指定寄存器提取一个8位值
{
    uint8_t a3 = 0x00;
    HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, (uint16_t) in_adr, I2C_MEMADD_SIZE_8BIT, &a3, 1, 0xff);
    return a3;
}

/**************************************************************
 * 函数名：uint16_t AS5600_readTwoBytes(uint8_t h_adr, uint8_t l_adr)
 * 函数功能：给定两个地址 各自读出一个字节
 * 输入参数：in_adr 待读高位数据的地址 in_adr_lo待读低位数据的地址
 * 输出参数：retVal读出的数据
 * 返回值：无
**************************************************************/
uint16_t AS5600_readTwoBytes(uint8_t h_adr, uint8_t l_adr) {
    uint16_t retVal = 0;
    uint8_t low = 0, high = 0;
    /* Read High Byte */
    high = AS5600_readOneByte(h_adr);
    high = high & 0x0f;
    /* Read Low Byte */
    low = AS5600_readOneByte(l_adr);
    //将两个八位 合成一个16位
    retVal = high << 8;
    retVal = (retVal | low);

    return retVal;//输出一个16位
}


/**************************************************************
 * 函数名：uint8_t AS5600_detectMagnet(void)
 * 函数功能：判断有无磁铁
 * 输入参数：无
 * 输出参数：1有磁铁 0无磁铁
 * 返回值：无
**************************************************************/
uint8_t AS5600_detectMagnet(void) {
    uint8_t retVal = 0;
    HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, (uint16_t) _stat, I2C_MEMADD_SIZE_8BIT, &retVal, 1, 0xff);
//    retVal = AS5600_readOneByte(_stat);

    if (retVal & 0x20)
        retVal = 1;

    return retVal;
}

/**************************************************************
 * 函数名：float AS5600_ReadSensorRawData(void)
 * 函数功能：读取传感器原始数据
 * 输入参数：无
 * 输出参数：有磁铁 传感器原始数据（范围0-4095）  无磁铁 返回上次的数据
 * 返回值：无
**************************************************************/
float AS5600_ReadSensorRawData(void) {
    static float data = 0;
    static uint8_t raw_data[2];
    if (AS5600_detectMagnet()) {
        /* Read High Byte */
        HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, (uint16_t) _raw_ang_hi, I2C_MEMADD_SIZE_8BIT, raw_data, 2, 0xff);
        //将两个八位 合成一个16位
        data = (raw_data[0] & 0x0f) << 8 | raw_data[1];
        /* Raw data reports 0 - 4095 segments */
        if (data < 0) {
            data += 4095.0f;
        }
    }
    return data;
}

