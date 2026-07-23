/**
 * @file br93l56fw.c
 * @brief GPIO Bit-Banging Driver for BR93L56FW EEPROM (Microwire Bus).
 * 
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2026 Liu Yu <f78fk@live.com>
 */

/* ---------------- Hardware Abstraction Layer (HAL) ---------------- */
/* TODO: Map these macros to your specific MCU/Platform GPIO registers */
#define EEPROM_CS_HIGH()    do { GPIO_SPI_CS = 1; } while(0)
#define EEPROM_CS_LOW()     do { GPIO_SPI_CS = 0; } while(0)
#define EEPROM_CLK_HIGH()   do { GPIO_SPI_SCK = 1; } while(0)
#define EEPROM_CLK_LOW()    do { GPIO_SPI_SCK = 0; } while(0)
#define EEPROM_MOSI_HIGH()  do { GPIO_SPI_MOSI = 1; } while(0)
#define EEPROM_MOSI_LOW()   do { GPIO_SPI_MOSI = 0; } while(0)
#define EEPROM_MISO_READ()  (GPIO_SPI_MISO)

/* Timing configuration: 500us delay generates approx. 1kHz clock frequency */
#define HALF_CLK_DELAY()    delay_us(500)

/* External delay functions provided by system architecture */
extern void delay_ms(uint16_t ms);
extern void delay_us(uint16_t us);

/* ---------------- Internal Low-Level Timing Functions ---------------- */

/**
 * @brief Transmits a specified number of bits via software SPI (MSB First).
 * @param value The data word containing bits to be sent.
 * @param bit_len Number of bits to transmit.
 */
static void eeprom_write_bits(uint16_t value, uint8_t bit_len)
{
    for (int8_t i = bit_len - 1; i >= 0; i--)
    {
        if (value & (1 << i)) {
            EEPROM_MOSI_HIGH();
        } else {
            EEPROM_MOSI_LOW();
        }

        HALF_CLK_DELAY();
        EEPROM_CLK_HIGH();
        HALF_CLK_DELAY();
        EEPROM_CLK_LOW();
    }
}

/**
 * @brief Receives a 16-bit data word from the EEPROM.
 * @return The 16-bit data read from MISO line.
 */
static uint16_t eeprom_read_bits16(void)
{
    uint16_t data = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        EEPROM_CLK_HIGH();
        HALF_CLK_DELAY(); /* Allow signal stabilization on the bus */

        data <<= 1;
        if (EEPROM_MISO_READ()) {
            data |= 0x0001;
        }

        EEPROM_CLK_LOW();
        HALF_CLK_DELAY();
    }
    return data;
}

/* ---------------- Public Application APIs ---------------- */

/**
 * @brief Initializes the software SPI GPIO pins to their default idle states.
 */
void br93l56fw_init(void)
{
    EEPROM_CLK_LOW();
    EEPROM_CS_LOW();
    EEPROM_MOSI_LOW();
}

/**
 * @brief Issues the Write Enable (WEN) command to unlock EEPROM write operations.
 * @note Sends 12-bit EWEN opcode sequence (0x13 and 0x01).
 */
void br93l56fw_write_enable(void)
{
    EEPROM_CS_HIGH();
    eeprom_write_bits(0x13, 6);
    eeprom_write_bits(0x01, 6);
    EEPROM_CS_LOW();
    delay_us(10); /* Tcs min delay before next instruction */
}

/**
 * @brief Reads a 16-bit word from the specified EEPROM address.
 * @param addr The 8-bit memory address to read from.
 * @return The 16-bit data word stored at the address.
 */
uint16_t br93l56fw_read(uint8_t addr)
{
    uint16_t data;

    EEPROM_CS_HIGH();
    /* Send READ Opcode: Start Bit '1' + Opcode '10' (Combined as 0x06, 4 bits) */
    eeprom_write_bits(0x06, 4);
    /* Send 8-bit memory address */
    eeprom_write_bits(addr, 8);
    /* Stream in the 16-bit data */
    data = eeprom_read_bits16();
    EEPROM_CS_LOW();

    return data;
}

/**
 * @brief Writes a 16-bit word to the specified EEPROM address.
 * @param addr The 8-bit memory address to write to.
 * @param data The 16-bit data word to be stored.
 */
void br93l56fw_write(uint8_t addr, uint16_t data)
{
    /* Always trigger Write Enable before write operation */
    br93l56fw_write_enable();

    EEPROM_CS_HIGH();
    /* Send WRITE Opcode: Start Bit '1' + Opcode '01' (Combined as 0x05, 4 bits) */
    eeprom_write_bits(0x05, 4);
    /* Send 8-bit memory address */
    eeprom_write_bits(addr, 8);
    /* Stream out the 16-bit data */
    eeprom_write_bits(data, 16);
    EEPROM_CS_LOW();

    /* Wait for the internal programming cycle to complete */
    delay_ms(10); 
}

/**
 * @brief Basic functional test routine for validation.
 */
void br93l56fw_test(void)
{
    uint16_t debug_data = 0;

    br93l56fw_init();
    br93l56fw_write_enable();

    /* Test Write and Read on address 0x00 */
    // br93l56fw_write(0x00, 0x1288);
    debug_data = br93l56fw_read(0x00);
    printf("debug_data: 0x%.4X\n", debug_data);

    while(1);
}
