#ifndef NRF24_DRIVER_H
#define NRF24_DRIVER_H

#include "stm32f446xx.h"
#include "main.h"

// NRF24L01 Register Addresses
#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05      
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define RPD             0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17

// Register Bits
/* CONFIG */


/* EN_AA */
#define ENAA_P0         0x01
#define ENAA_P1         0x02
#define ENAA_P2         0x04
#define ENAA_P3         0x08
#define ENAA_P4         0x10
#define ENAA_P5         0x20

/* EN_RXADDR */

/* SETUP_RETR */
// Auto Retransmit Delay - ARD(uS)
#define ARD_250        0000
#define ARD_500        0001
#define ARD_750        0010
#define ARD_1000       0011
#define ARD_1250       0100
#define ARD_1500       0001
#define ARD_1750       0110
#define ARD_2000       0111
#define ARD_2250       1000
#define ARD_2500       1001
#define ARD_2750       1010
#define ARD_3000       1011
#define ARD_3250       1100
#define ARD_3500       1101
#define ARD_3750       1110
#define ARD_4000       1111

// Auto Retransmit Count - ARC
#define ARC_0        0000
#define ARC_1        0001
#define ARC_2        0010
#define ARC_3       0011
#define ARC_4       0100
#define ARC_5       0001
#define ARC_6       0110
#define ARC_7       0111
#define ARC_8       1000
#define ARC_9       1001
#define ARC_10       1010
#define ARC_11       1011
#define ARC_12       1100
#define ARC_13       1101
#define ARC_14       1110
#define ARC_15       1111

/* SETUP_AW (bytes)*/
// Address Width
#define AW_3   0x01
#define AW_4  0x02
#define AW_5   0x03

/* */



// Commands
#define W_REGISTER 0b100000
#define R_REGISTER 0b000000
#define R_RX_PAYLOAD 0b01100001
#define W_TX_PAYLOAD 0b10100000
#define FLUSH_TX 0b11100001
#define FLUSH_RX 0b11100010
#define REUSE_TX_PL 0b11100011
#define R_RX_PL_WID 0b01100000
#define W_TX_PAYLOAD_NOACK 0b10110000
#define W_ACK_PAYLOAD 0b10101000
#define NOP_CMD 0b11111111

/* GPIO Control */
void nrf_csn_low(void);
void nrf_csn_high(void);
void nrf_ce_low(void);
void nrf_ce_high(void);

/* SPI Transfer */
uint8_t nrf_spi_transfer(uint8_t data);
void nrf_write_reg(uint8_t reg, uint8_t value);
uint8_t nrf_read_reg(uint8_t reg);
void nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len);

void nrf_init(SPI_HandleTypeDef *hspi);
void nrf_init_tx(void);
void nrf_init_rx(void);

void nrf_send(uint8_t *data, uint8_t len);
void nrf_read(uint8_t *data, uint8_t len);
uint8_t nrf_data_available(void);

#endif /* NRF24_DRIVER_H */