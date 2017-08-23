#ifndef CC85XX_EHIF_H
#define CC85XX_EHIF_H

#include <stm8s.h>

/* EHIF Cmd Type Defines */
typedef enum spi_cmd_req_type_e
{
    CMD_NONE                  = 0x00,
    CMD_DI_GET_CHIP_INFO      = 0x1F,
    CMD_DI_GET_DEVICE_INFO    = 0x1E,
    
    CMD_EHC_EVT_MASK          = 0x1A,
    CMD_EHC_EVT_CLR           = 0x19,
    
    CMD_NVM_DO_SCAN           = 0x08,
    CMD_NVM_DO_JOIN           = 0x09,
    CMD_NVM_GET_STATUS        = 0x0A,
    CMD_NVM_ACH_SET_USAGE     = 0x0B,
    CMD_NVM_CONTROL_ENABLE    = 0x0C,
    CMD_NVM_CONTROL_SIGNAL    = 0x0D,
    
    CMD_DSC_TX_DATAGRAM       = 0x04,  
    CMD_DSC_RX_DATAGRAM       = 0x05,
    
    CMD_PM_SET_STATE          = 0x1C,
    CMD_PM_GET_DATA           = 0x1D,
    
    CMD_VC_GET_VOLUME         = 0x16,
    CMD_VC_SET_VOLUME         = 0x17,
    
    CMD_PS_RF_STATS           = 0x10,
    CMD_PS_AUDIO_STATS        = 0x11,
    
    CMD_CAL_SET_DATA          = 0x28,
    CMD_CAL_GET_DATA          = 0x29,
    
    CMD_NVS_GET_DATA          = 0x2B,
    CMD_NVS_SET_DATA          = 0x2C,

    CMD_RC_SET_DATA           = 0x2D,
    CMD_RC_GET_DATA           = 0x2E,
    
    CMD_RFT_TXTST_CW          = 0x15,
    CMD_RFT_TXTST_PN          = 0x14,
    CMD_RFT_RXTST             = 0x26,
    CMD_RFT_NWKSIM            = 0x00,
    CMD_RFT_TXPER             = 0x13,
    CMD_RFT_RXPER             = 0x12,
    
    CMD_AT_GEN_TONE           = 0x20,
    CMD_AT_DET_TONE           = 0x21,
    
    CMD_IOTST_INPUT           = 0x22,
    CMD_IOTST_OUTPUT          = 0x23,
} SPI_CMD_REQ_TYPE_E;

/* SPI Status Word Define */
typedef struct spi_status_word_s
{
    uint16_t    evt_sr_chg          :1;
    uint16_t    evt_nwk_chg         :1;
    uint16_t    evt_ps_chg          :1;
    uint16_t    evt_vol_chg         :1;
    uint16_t    evt_spi_error       :1;
    uint16_t    evt_dsc_reset       :1;
    uint16_t    evt_dsc_tx_avail    :1;
    uint16_t    evt_dsc_rx_avail    :1;
    uint16_t    wasp_conn           :1;
    uint16_t    pwr_state           :3;
    uint16_t                        :3;
    uint16_t    cmdreq_rdy          :1;
} SPI_STATUS_S;

typedef enum ehif_magic_num_e {
    EHIF_MAGIC_CMD_REQ      = 0x3,      /* 0b11 */
    EHIF_MAGIC_WRITE        = 0x8,      /* 0b1000 */
    EHIF_MAGIC_READ         = 0x9,      /* 0b1001 */
    EHIF_MAGIC_READBC       = 0xa,      /* 0b1010 */
} EHIF_MAGIC_NAM_E;

/* SPI Data Head Defines */
typedef struct spi_head_CMD_REQ_s
{
    uint16_t    data_len            :8;
    uint16_t    cmd_type            :6;
    uint16_t    magic_num           :2;
} SPI_HEAD_CMD_REQ_S;

typedef struct spi_head_READ_WRITE_s
{
    uint16_t    data_len            :12;
    uint16_t    magic_num           :4;
} SPI_HEAD_READ_WRITE_S;

#define EHIF_HEAD_SIZE  sizeof(SPI_HEAD_CMD_REQ_S)

void spi_init(void);

void _basic_operation(EHIF_MAGIC_NAM_E magic,
        uint16_t cmd, uint16_t *len, void *data, SPI_STATUS_S *status);

/* Basic SPI Operations Function */
void basic_SYS_RESET(void);
void basic_GET_STATUS(void *status);


#define basic_CMD_REQ(cmd, len, pdata, pstatus) \
{                                                                           \
    uint16_t l = (len);                                                     \
    _basic_operation(EHIF_MAGIC_CMD_REQ, (cmd), &l, (pdata), (pstatus));    \
}

#define basic_WRITE(len, pdata, pstatus) \
{                                                                           \
    uint16_t l = (len);                                                     \
    _basic_operation(EHIF_MAGIC_WRITE, CMD_NONE, &l, (pdata), (pstatus));   \
}

#define basic_READ(len, pdata, pstatus) \
{                                                                           \
    uint16_t l = (len);                                                     \
    _basic_operation(EHIF_MAGIC_READ, CMD_NONE, &l, (pdata), (pstatus));    \
}

#define basic_READBC(plen, pdata, pstatus) \
    _basic_operation(EHIF_MAGIC_READBC, CMD_NONE, (plen), (pdata), (pstatus))


#endif /* CC85XX_EHIF_H */
