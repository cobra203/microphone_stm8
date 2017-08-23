#include <string.h>
#include <cc85xx_ehif_host_ctrl.h>
#include <stm8s_debug.h>

#define VOLUME_IS_IN_VOL_OUTPUT     0
#define VOLUME_IS_IN_VOL_INPUT      1

#define VOLUME_IS_LOCAL_REMOTE      0
#define VOLUME_IS_LOCAL_LOCAL       1

typedef struct cc85xx_set_volume_s
{
    uint8_t     is_local    :1;
    uint8_t     is_in_vol   :1;
    uint8_t                 :6;
    uint8_t     log_channel :4;
    uint8_t     set_op      :2;
    uint8_t     mute_op     :2;   
    uint16_t    volume      :16;
} CC85XX_SET_VOLUME_S;

typedef struct cc85xx_get_volume_s
{
    uint8_t     is_local    :1;
    uint8_t     is_in_vol   :1;
    uint8_t                 :6;
} CC85XX_GET_VOLUME_S;

typedef struct cc85xx_nwm_do_join_s
{
    uint16_t    join_to;
    uint32_t    device_id;
    uint32_t    manf_id;
    uint32_t    prod_id_mask;
    uint32_t    prod_id_ref;
} CC85XX_NWM_DO_JOIN_S;

typedef struct cc85xx_nwm_ach_set_usage_s
{
    uint8_t     usage_s[16];
} CC85XX_NWM_ACH_SET_USAGE_S;

typedef struct rc_set_data_head_s
{
    uint8_t     cmd_count   :3;
    uint8_t     keyb_count  :3;
    uint8_t     ext_sel     :2;
} RC_SET_DATA_HEAD_S;

typedef struct rc_set_data_s
{
    RC_SET_DATA_HEAD_S  head;
    uint8_t             data[12];
} RC_SET_DATA_S;


int ehif_DI_GET_DEVICE_INFO(CC85XX_DEV_INFO_S *dev_info)
{
    SPI_STATUS_S    status = {0};
    
    basic_CMD_REQ(CMD_DI_GET_DEVICE_INFO, 0, NULL, &status);
    basic_READ(sizeof(CC85XX_DEV_INFO_S), (uint8_t *)dev_info, &status);

    return EHIF_SUCCESS;
}

int ehif_PM_SET_STATE(uint8_t pwr_state)
{
    SPI_STATUS_S    status = {0};
    basic_CMD_REQ(CMD_PM_SET_STATE, 1, &pwr_state, &status);

    return EHIF_SUCCESS;
}

int ehif_VC_SET_VOLUME(uint8_t mute_ops, uint8_t set_ops, uint16_t volume)
{
    CC85XX_SET_VOLUME_S set_volume = {0};
    SPI_STATUS_S        status = {0};
  
    set_volume.is_in_vol    = VOLUME_IS_IN_VOL_OUTPUT;
    set_volume.is_local     = VOLUME_IS_LOCAL_REMOTE;
#if 1
    set_volume.is_in_vol    = VOLUME_IS_IN_VOL_OUTPUT;
    set_volume.is_local     = VOLUME_IS_LOCAL_LOCAL;
#endif

    
    set_volume.mute_op      = mute_ops;
    set_volume.set_op       = set_ops;
    set_volume.log_channel  = 0;
    
    set_volume.volume       = volume;

    basic_CMD_REQ(CMD_VC_SET_VOLUME, sizeof(CC85XX_SET_VOLUME_S), &set_volume, &status);

    return EHIF_SUCCESS;   
}

int ehif_VC_GET_VOLUME(uint16_t *volume)
{
    CC85XX_GET_VOLUME_S get_volume = {0};
    SPI_STATUS_S        status = {0};
    
    get_volume.is_in_vol    = VOLUME_IS_IN_VOL_OUTPUT;
    get_volume.is_local     = VOLUME_IS_LOCAL_REMOTE;
    
#if 1
    get_volume.is_in_vol    = VOLUME_IS_IN_VOL_OUTPUT;
    get_volume.is_local     = VOLUME_IS_LOCAL_LOCAL;
#endif

    basic_CMD_REQ(CMD_VC_GET_VOLUME, 1, &get_volume, &status);
    basic_READ(sizeof(uint16_t), volume, &status);

    return EHIF_SUCCESS;
}

int ehif_NVS_GET_DATA(uint8_t slot, uint32_t *nwm_id)
{
    SPI_STATUS_S    status = {0};

    basic_CMD_REQ(CMD_NVS_GET_DATA, sizeof(uint8_t), &slot, &status);
    basic_READ(sizeof(uint32_t), nwm_id, &status);

    return EHIF_SUCCESS;
}

int ehif_NVS_SET_DATA(uint8_t slot, uint32_t nwm_id)
{
    uint8_t         set_data[5] = {slot};
    SPI_STATUS_S    status = {0};

    memcpy(&set_data[1], &nwm_id, sizeof(uint32_t));

    basic_CMD_REQ(CMD_NVS_SET_DATA, sizeof(set_data), set_data, &status);

    return EHIF_SUCCESS;
}


int ehif_NWM_DO_JOIN(uint16_t join_to, uint32_t device_id, uint32_t manf_id)
{
    CC85XX_NWM_DO_JOIN_S    do_join = {0};
    SPI_STATUS_S            status = {0};

    do_join.join_to     = 0x0fff & join_to;
    do_join.device_id   = device_id;
    do_join.manf_id     = manf_id;

    basic_CMD_REQ(CMD_NVM_DO_JOIN, sizeof(do_join), &do_join, &status);

    return EHIF_SUCCESS;
}

int ehif_NWM_GET_STATUS(CC85XX_NWM_GET_STATUS_S *nwm_status)
{
    SPI_STATUS_S    status = {0};
    uint16_t        readlen = 0;

    basic_CMD_REQ(CMD_NVM_GET_STATUS, 0, NULL, &status);
    basic_READBC(&readlen, (uint8_t *)nwm_status, &status);

    if(readlen == sizeof(CC85XX_NWM_GET_STATUS_S)) {
        return EHIF_SUCCESS;
    }
    return EHIF_FAILURE;
}

int ehif_NWM_ACH_SET_USAGE(uint8_t single_chnn)
{
    CC85XX_NWM_ACH_SET_USAGE_S  set_usage;
    SPI_STATUS_S                status = {0};
    uint8_t i = 0;

    for(i = 0; i < sizeof(set_usage); i++) {
        set_usage.usage_s[i] = 0xff;
    }

    set_usage.usage_s[single_chnn] = 0;

    basic_CMD_REQ(CMD_NVM_ACH_SET_USAGE, sizeof(set_usage), &set_usage, &status);

    return EHIF_SUCCESS;
}

int ehif_RC_SET_DATA(uint8_t *data, uint8_t count)
{
    RC_SET_DATA_S   set_data = {0};
    SPI_STATUS_S    status = {0};

    if(count > 12) {
        return EHIF_FAILURE;
    }
    set_data.head.cmd_count = count;
    memcpy(set_data.data, data, count);

    basic_CMD_REQ(CMD_RC_SET_DATA, sizeof(set_data), &set_data, &status);

    return EHIF_SUCCESS;
}

