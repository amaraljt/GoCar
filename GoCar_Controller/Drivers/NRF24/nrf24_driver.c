#include "nrf24_driver.h"

#define NRF_CSN_PORT GPIOB
#define NRF_CSN_PIN GPIO_PIN_1
#define NRF_CE_PORT GPIOB
#define NRF_CE_PIN GPIO_PIN_0
#define NRF_CHANNEL 76

static SPI_HandleTypeDef *g_nrf_spi;

/* Set CSN low */
void nrf_csn_low(void)
{
    HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_RESET);
}

/* Set CSN high */
void nrf_csn_high(void)
{
    HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_SET);
}

/* Set CE low */
void nrf_ce_low(void)
{
    HAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_RESET);
}

/* Set CE high */
void nrf_ce_high(void)
{
    HAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_SET);
}


uint8_t nrf_spi_transfer(uint8_t data)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(g_nrf_spi, &data, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

/* Write data to a register */
void nrf_write_reg(uint8_t reg, uint8_t value)
{
    nrf_csn_low();
    nrf_spi_transfer(W_REGISTER | reg);   // W_REGISTER
    nrf_spi_transfer(value);
    nrf_csn_high();
}

/* Read data from a register */
uint8_t nrf_read_reg(uint8_t reg)
{
    uint8_t value;

    nrf_csn_low();
    nrf_spi_transfer(reg);
    value = nrf_spi_transfer(0xFF);
    nrf_csn_high();

    return value;
}

/* Write buffer of data to a register */
void nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    nrf_csn_low();
    nrf_spi_transfer(W_REGISTER | reg);
    for(int i = 0; i < len; i++)
        nrf_spi_transfer(buf[i]);
    nrf_csn_high();
}

/* Store spi handle pointer */
void nrf_init(SPI_HandleTypeDef *hspi)
{
    g_nrf_spi = hspi;
}

/* Configure nrf to transmit */
void nrf_init_tx(void)
{
    nrf_ce_low();
    HAL_Delay(5);

    nrf_write_reg(SETUP_AW, AW_5);      // Sets 5-byte address
    nrf_write_reg(RF_CH, NRF_CHANNEL);           // Sets channel
    nrf_write_reg(RF_SETUP, 0x0E);      // 2Mbps, max power
    nrf_write_reg(EN_AA, ENAA_P0);         /* Data pipe 0 auto-ack */
    nrf_write_reg(SETUP_RETR, ARD_500 | ARC_10);    /* Auto Retry */

    uint8_t addr[5] = {0x78,0x78,0x78,0x78,0x78};
    nrf_write_buf(TX_ADDR, addr, 5);
    nrf_write_buf(RX_ADDR_P0, addr, 5);

    nrf_write_reg(RX_PW_P0, 2);

    nrf_write_reg(STATUS, 0x70);

    nrf_write_reg(CONFIG, 0x0E);
    HAL_Delay(2);
}

/* Configure NRF to listen */
void nrf_init_rx(void)
{
    nrf_ce_low();
    HAL_Delay(5);

    nrf_write_reg(SETUP_AW, AW_5);
    nrf_write_reg(RF_CH, NRF_CHANNEL);
    nrf_write_reg(RF_SETUP, 0x0E);
    nrf_write_reg(EN_AA, ENAA_P0);
    nrf_write_reg(SETUP_RETR, 0x1A);

    uint8_t addr[5] = {0x78,0x78,0x78,0x78,0x78};
    nrf_write_buf(RX_ADDR_P0, addr, 5);

    nrf_write_reg(RX_PW_P0, 2);

    nrf_write_reg(STATUS, 0x70);

    nrf_write_reg(CONFIG, 0x0F);   // PRIM_RX = 1
    HAL_Delay(2);

    nrf_ce_high();  // start listening
}

void nrf_send(uint8_t *data, uint8_t len)
{
    nrf_ce_low();

    nrf_csn_low();
    nrf_spi_transfer(0xA0);  // W_TX_PAYLOAD
    for(int i = 0; i < len; i++)
        nrf_spi_transfer(data[i]);
    nrf_csn_high();

    nrf_ce_high();
    HAL_Delay(1);
    nrf_ce_low();

    // Wait for TX_DS or MAX_RT
    while(!(nrf_read_reg(STATUS) & 0x30));

    nrf_write_reg(STATUS, 0x30); // clear flags
}

void nrf_read(uint8_t *data, uint8_t len)
{
    nrf_csn_low();
    nrf_spi_transfer(0x61);  // R_RX_PAYLOAD
    for(int i = 0; i < len; i++)
        data[i] = nrf_spi_transfer(0xFF);
    nrf_csn_high();

    nrf_write_reg(STATUS, 0x40); // clear RX_DR
}

uint8_t nrf_data_available(void)
{
    return (nrf_read_reg(STATUS) & 0x40);
}