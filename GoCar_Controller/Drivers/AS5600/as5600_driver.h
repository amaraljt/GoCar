#ifndef AS5600_DRIVER_H
#define AS5600_DRIVER_H

#include "stm32f446xx.h"
#include <stdint.h>
#include "main.h"

uint16_t read_raw_angle(I2C_HandleTypeDef *hi2c);
float get_angle_degrees(I2C_HandleTypeDef *hi2c);

#endif /* AS5600_DRIVER_H */