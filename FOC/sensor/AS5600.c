#include "AS5600.h"
#include "i2c.h"

void AS5600_init() {
    uint8_t data = 0x03;
    HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, _conf_hi, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);

    return;
}

/**************************************************************
 * ��������uint8_t AS5600_readOneByte(uint8_t in_adr)
 * �������ܣ�������ַ��һ���ֽ�
 * ���������in_adr �������ݵĵ�ַ
 * ���������retVal����������
 * ����ֵ����
**************************************************************/
uint8_t AS5600_readOneByte(uint8_t in_adr)//��һ���ֽں�������5600��ָ���Ĵ�����ȡһ��8λֵ
{
    uint8_t a3 = 0x00;
    HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, (uint16_t) in_adr, I2C_MEMADD_SIZE_8BIT, &a3, 1, 0xff);
    return a3;
}

/**************************************************************
 * ��������uint16_t AS5600_readTwoBytes(uint8_t h_adr, uint8_t l_adr)
 * �������ܣ�����������ַ ���Զ���һ���ֽ�
 * ���������in_adr ������λ���ݵĵ�ַ in_adr_lo������λ���ݵĵ�ַ
 * ���������retVal����������
 * ����ֵ����
**************************************************************/
uint16_t AS5600_readTwoBytes(uint8_t h_adr, uint8_t l_adr) {
    uint16_t retVal = 0;
    uint8_t low = 0, high = 0;
    /* Read High Byte */
    high = AS5600_readOneByte(h_adr);
    high = high & 0x0f;
    /* Read Low Byte */
    low = AS5600_readOneByte(l_adr);
    //��������λ �ϳ�һ��16λ
    retVal = high << 8;
    retVal = (retVal | low);

    return retVal;//���һ��16λ
}


/**************************************************************
 * ��������uint8_t AS5600_detectMagnet(void)
 * �������ܣ��ж����޴���
 * �����������
 * ���������1�д��� 0�޴���
 * ����ֵ����
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
 * ��������float AS5600_ReadSensorRawData(void)
 * �������ܣ���ȡ������ԭʼ����
 * �����������
 * ����������д��� ������ԭʼ���ݣ���Χ0-4095��  �޴��� �����ϴε�����
 * ����ֵ����
**************************************************************/
float AS5600_ReadSensorRawData(void) {
    static float data = 0;
    static uint8_t raw_data[2];
    if (AS5600_detectMagnet()) {
        /* Read High Byte */
        HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, (uint16_t) _raw_ang_hi, I2C_MEMADD_SIZE_8BIT, raw_data, 2, 0xff);
        //��������λ �ϳ�һ��16λ
        data = (raw_data[0] & 0x0f) << 8 | raw_data[1];
        /* Raw data reports 0 - 4095 segments */
        if (data < 0) {
            data += 4095.0f;
        }
    }
    return data;
}

