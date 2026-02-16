#include <stdio.h>
#include "lcd_driver.h"
#include "main.h"
#include <string.h>

#define SLAVE_ADDRESS_LCD (0x27 << 1)

static I2C_HandleTypeDef *g_lcd_i2c;
//static uint8_t lcd_addr;
//static uint8_t backlight = 0x08;  // backlight ON

#if 0
static void lcd_write(I2C_HandleTypeDef *hi2c, uint8_t data)
{
    HAL_I2C_Master_Transmit(hi2c, lcd_addr << 1, &data, 1, HAL_MAX_DELAY);
}

static void lcd_enable(uint8_t data)
{
    uint8_t temp = data | 0x04;   // E = 1
    lcd_write(temp);
    HAL_Delay(1);

    temp &= ~0x04;                // E = 0
    lcd_write(temp);
    HAL_Delay(1);
}

static void lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble & 0xF0) | backlight;

    if(rs)
        data |= 0x01;  // RS = 1 (data)
    else
        data &= ~0x01; // RS = 0 (command)

    lcd_enable(data);
}

static void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_send_nibble(byte & 0xF0, rs);
    lcd_send_nibble((byte << 4) & 0xF0, rs);
}

void lcd_init(I2C_HandleTypeDef *hi2c, uint8_t address)
{
    lcd_i2c = hi2c;
    lcd_addr = address;

    HAL_Delay(50);

    lcd_send_nibble(0x30, 0);
    HAL_Delay(5);
    lcd_send_nibble(0x30, 0);
    HAL_Delay(1);
    lcd_send_nibble(0x30, 0);
    HAL_Delay(10);

    lcd_send_nibble(0x20, 0); // 4-bit mode
    HAL_Delay(10);

    lcd_send_byte(0x28, 0); // 2 lines, 5x8 font
    lcd_send_byte(0x0C, 0); // display on
    lcd_send_byte(0x06, 0); // entry mode
    lcd_send_byte(0x01, 0); // clear
    HAL_Delay(5);
}

void lcd_clear(void)
{
    lcd_send_byte(0x01, 0);
    HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_send_byte(addr, 0);
}

void lcd_print(char *str)
{
    while(*str)
    {
        lcd_send_byte((uint8_t)(*str), 1);
        str++;
    }
}
#else
void lcd_send_cmd(char cmd)
{
    char data_u, data_l;
    data_u = (cmd & 0xF0);           // extract upper 4 bits
    data_l = ((cmd << 4) & 0xF0);    // extract lower 4 bits

    uint8_t data_t[4];

    // send upper 4 bits with enable pulse
    data_t[0] = data_u | 0x0C;   // EN=1, RS=0  -> bxxxx1100
    data_t[1] = data_u | 0x08;   // EN=0, RS=0  -> bxxxx1000

    // send lower 4 bits with enable pulse
    data_t[2] = data_l | 0x0C;   // EN=1, RS=0  -> bxxxx1100
    data_t[3] = data_l | 0x08;   // EN=0, RS=0  -> bxxxx1000

    HAL_I2C_Master_Transmit(g_lcd_i2c, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
    HAL_Delay(2);
}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[1] = data_u|0x09;  //en=0, rs=1 -> bxxxx1001
	data_t[2] = data_l|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[3] = data_l|0x09;  //en=0, rs=1 -> bxxxx1001
	HAL_I2C_Master_Transmit (g_lcd_i2c, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
    HAL_Delay(1);
}

void lcd_init(I2C_HandleTypeDef *hi2c1)
{
    g_lcd_i2c = hi2c1;
    // 4 bit initialisation
    HAL_Delay(50);  // wait for >40ms
    lcd_send_cmd (0x30);
    HAL_Delay(5);  // wait for >4.1ms
    lcd_send_cmd (0x30);
    HAL_Delay(1);  // wait for >100us
    lcd_send_cmd (0x30);
    HAL_Delay(10);
    lcd_send_cmd (0x20);  // 4bit mode
    HAL_Delay(10);

    // display initialisation
    lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
    HAL_Delay(1);
    lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
    HAL_Delay(1);
    lcd_send_cmd (0x01);  // clear display
    HAL_Delay(2);
    lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
    HAL_Delay(1);
    lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string(char *str)
{
    while (*str) lcd_send_data (*str++);
}

void lcd_put_cur(uint8_t row, uint8_t col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd (col);
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}
#endif