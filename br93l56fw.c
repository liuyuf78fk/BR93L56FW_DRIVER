
#define SPI_SCK_1		GPIO_SPI_SCK = 1
#define SPI_SCK_0		GPIO_SPI_SCK = 0
#define SPI_CS_1		GPIO_SPI_CS = 1
#define SPI_CS_0		GPIO_SPI_CS = 0
#define SPI_MOSI_1          	GPIO_SPI_MOSI = 1
#define SPI_MOSI_0          	GPIO_SPI_MOSI = 0
#define SPI_READ_MISO       	GPIO_SPI_MISO

void delay_ms(uint16_t ms)
{

}

void delay_us(uint16_t us)
{

}

void spi_init(void)
{
    SPI_SCK_0;
    SPI_CS_0;
    SPI_MOSI_0;
}

void br93l56fw_send_addr(uint8_t send_addr)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        if(send_addr & 0x80)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        send_addr <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }
}

void br93l56fw_send_data(uint16_t send_data)
{
    int i;

    for(i = 0; i < 16; i++)
    {
        if(send_data & 0x8000)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        send_data <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }
}

uint16_t br93l56fw_receive(void)
{
    uint16_t receive_data = 0;
    uint8_t i;

    for(i = 0; i < 16; i++)
    {
        SPI_SCK_1;
        delay_us(500);
        receive_data <<= 1;
        if(SPI_READ_MISO)
        {
            receive_data ++;
        }
        SPI_SCK_0;
        delay_us(500);

    }
    return receive_data;
}

void br93l56fw_wen(void)
{
    SPI_MOSI_0;
    SPI_CS_1;
    uint8_t i;
    uint8_t wen_code = 0x13;
    for(i = 0; i < 6; i++)
    {
        if(wen_code & 0x20)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        wen_code <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }

    wen_code = 0x01;
    for(i = 0; i < 6; i++)
    {
        if(wen_code & 0x20)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        wen_code <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }
    SPI_CS_0;
}

uint16_t br93l56fw_read(uint8_t read_addr)
{
    uint16_t read_data;
    uint8_t i;
    uint8_t read_code = 0x06;
    SPI_MOSI_0;
    SPI_CS_1;
    for(i = 0; i < 4; i++)
    {
        if(read_code & 0x08)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        read_code <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }

    br93l56fw_send_addr(read_addr);

    read_data = br93l56fw_receive();
    SPI_CS_0;
    return read_data;
}

void br93l56fw_write(uint8_t write_addr, uint16_t write_data)
{
    uint8_t i;
    uint8_t write_code = 0x05;
    br93l56fw_wen();
    SPI_MOSI_0;
    SPI_CS_1;

    for(i = 0; i < 4; i++)
    {
        if(write_code & 0x8)
        {
            SPI_MOSI_1;
        }
        else
        {
            SPI_MOSI_0;
        }
        write_code <<= 1;
        delay_us(500);
        SPI_SCK_1;
        delay_us(500);
        SPI_SCK_0;
    }
    br93l56fw_send_addr(write_addr);
    br93l56fw_send_data(write_data);
    SPI_CS_0;
    delay_ms(6);

}

void br93l56fw_erase(uint8_t erase_addr, uint8_t erase_length)
{

}

void br93l56fw_test(void)
{
    spi_init();
    br93l56fw_wen();

    uint16_t debug_data = 0;
    //br93l56fw_write(0x0,0x1288);
    debug_data = br93l56fw_read(0x0);
    printf("debug_data:0x%.4X\n", debug_data);
    while(1);
}

