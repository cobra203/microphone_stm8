#include <string.h>
#include <stm8s_spi.h>
#include <cc85xx_ehif.h>
#include <stm8s_debug.h>

#define _WAIT_SPI_STAUTS_SET(flag)     while(SPI_GetFlagStatus(flag) == RESET)

#define WAIT_SEND_EN()     _WAIT_SPI_STAUTS_SET(SPI_FLAG_TXE)
#define WAIT_RECV_EN()     _WAIT_SPI_STAUTS_SET(SPI_FLAG_RXNE)

#define WAIT_MISO_SET()     while(GPIO_ReadInputPin(GPIOC, GPIO_PIN_7) == RESET);

typedef enum spi_enable_flag_e
{
    SPI_WAIT_NONE,
    SPI_WAIT_MISO_IS_SET,
} SPI_ENABLE_FLAG_E;

static void spi_enable(SPI_ENABLE_FLAG_E flag)
{
    disableInterrupts();
    
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);

    GPIO_WriteLow(GPIOA, GPIO_PIN_3);

    if(flag == SPI_WAIT_MISO_IS_SET) {
        WAIT_MISO_SET();
    }

    SPI_Cmd(ENABLE);
    
    enableInterrupts();
}

static void spi_disable(void)
{
    disableInterrupts();

    SPI_Cmd(DISABLE);

    GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
 
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOA);

    enableInterrupts();
}

static void _spi_transfer(const void *send_data, uint8_t send_len, void *recv_data, uint8_t recv_len)
{
    uint8_t         send_sz     = send_len;
    uint8_t         recv_sz     = recv_len;
    uint8_t         times       = send_len > recv_len ? send_len : recv_len;
    const uint8_t   *sendbuf    = (const uint8_t *)send_data;
    uint8_t         *recvbuf    = (uint8_t *)recv_data;

    while(times--) {
        WAIT_SEND_EN();
        send_len ? SPI_SendData(sendbuf[send_sz - send_len--]) : SPI_SendData(1);
     
        WAIT_RECV_EN();
        recv_len ? recvbuf[recv_sz - recv_len--] = SPI_ReceiveData() : SPI_ReceiveData();      
    }
}

void spi_init(void)
{
    SPI_DeInit();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
            SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
}

void _basic_operation(EHIF_MAGIC_NAM_E magic,
                        uint16_t cmd, uint16_t *len, void *data, SPI_STATUS_S *status)
{
    SPI_HEAD_CMD_REQ_S      cmd_head    = {0};
    SPI_HEAD_READ_WRITE_S   rw_head     = {0};
    void                    *phead      = NULL;

    cmd_head.cmd_type   = cmd;
    cmd_head.data_len   = rw_head.data_len  = *len;
    cmd_head.magic_num  = rw_head.magic_num = magic;

    memset(status, 0, sizeof(SPI_STATUS_S));
    
    phead = (magic == EHIF_MAGIC_CMD_REQ) ? (void *)&cmd_head : (void *)&rw_head;

    /* Start Communicate */
    spi_enable(SPI_WAIT_MISO_IS_SET);

    _spi_transfer(phead, EHIF_HEAD_SIZE, status, sizeof(SPI_STATUS_S));

    switch(magic) {
    case EHIF_MAGIC_CMD_REQ:
    case EHIF_MAGIC_WRITE:
        if(*len) {
            _spi_transfer(data, *len, NULL, 0);
        }
        break;
    case EHIF_MAGIC_READBC:
        _spi_transfer(NULL, 0, len, 2);
    case EHIF_MAGIC_READ:
        if(*len) {
            _spi_transfer(NULL, 0, data, *len);
        }
        break;
    }

    /* End Communicate */
    spi_disable();

#if defined(DUMP_ENABLE)
    switch(cmd) {
    case CMD_RC_SET_DATA:
        datadump("s", phead, EHIF_HEAD_SIZE);
        datadump("r", status, sizeof(SPI_STATUS_S));

        if(*len) {
            switch(magic) {
            case EHIF_MAGIC_CMD_REQ:
            case EHIF_MAGIC_WRITE:
                datadump("s", data, *len);
                break;
            case EHIF_MAGIC_READBC:   
            case EHIF_MAGIC_READ:
                datadump("r", data, *len);
                break;
            }
        }
        break;
    }
#endif  
    
}

void basic_GET_STATUS(void *status)
{
    uint8_t buf[2] = {0x80, 0};

    spi_enable(SPI_WAIT_NONE);
    _spi_transfer(&buf, sizeof(buf), status, sizeof(SPI_STATUS_S));
    spi_disable();

#if defined(DUMP_ENABLE3)
    datadump("s", buf, sizeof(buf));
    datadump("r", status, sizeof(SPI_STATUS_S));
#endif

}

void basic_SYS_RESET(void)
{
    uint8_t buf[2] = {0xbf, 0xff};
    uint8_t recvbuf[2] = {0};

    /* Start Communicate */
    spi_enable(SPI_WAIT_NONE);

    asm("nop");

    _spi_transfer(buf, sizeof(buf), recvbuf, sizeof(recvbuf));
    SPI_Cmd(DISABLE);

    asm("nop");
    GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
    asm("nop");
    GPIO_WriteLow(GPIOA, GPIO_PIN_3);
    asm("nop");

    WAIT_MISO_SET();
    asm("nop");

    /* End Communicate */
    spi_disable();

#if defined(DUMP_ENABLE3)
    datadump("s", buf, sizeof(buf));
    datadump("r", recvbuf, sizeof(recvbuf));
#endif
}

