#include "as5600_driver.h"
#include "stm32f4xx_hal_i2c.h"

#define SLAVE_ADDRESS_AS5600 (0x36 << 1)
#define RAW_ANGLE_REG_AS5600 0x0C

uint16_t read_raw_angle(I2C_HandleTypeDef *hi2c)
{
    uint8_t buffer[2];
    uint16_t angle = 0;

    HAL_I2C_Mem_Read(hi2c, 
        SLAVE_ADDRESS_AS5600, // i2c perihperal
        RAW_ANGLE_REG_AS5600, // device addr
        I2C_MEMADD_SIZE_8BIT, // register addr
        buffer, // where to store result
        2, // number of bytes
        HAL_MAX_DELAY); // timeout

    angle = (buffer[0] << 8 | buffer[1]);

    return angle;
}

float get_angle_degrees(I2C_HandleTypeDef *hi2c)
{
    uint16_t raw = read_raw_angle(hi2c);
    return (raw * 360.0f) / 4096.0f;
}
