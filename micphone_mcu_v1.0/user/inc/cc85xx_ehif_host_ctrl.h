#ifndef CC85XX_EHIF_HOST_CTRL_H
#define CC85XX_EHIF_HOST_CTRL_H

#include <cc85xx_ehif.h>

#define EHIF_SUCCESS                0
#define EHIF_FAILURE                -1

#define VOLUME_MUTE_OP_NONE         0
#define VOLUME_MUTE_OP_MUTE         1
#define VOLUME_MUTE_OP_UNMUTE       2

#define VOLUME_SET_OP_ABSOLUTE      1
#define VOLUME_SET_OP_RELATIVE      2
#define VOLUME_SET_OP_LOGICAL_CHANNEL_OFFSET 3

typedef struct cc85xx_device_info_s
{
    uint32_t    device_id;
    uint32_t    manf_id;
    uint32_t    prod_id;
} CC85XX_DEV_INFO_S;

typedef struct ach_info_s
{
    uint8_t     format_b    :3;
    uint8_t     active_b    :1;
    uint8_t     format_a    :3;
    uint8_t     active_a    :1;
} ACH_INFO_S;

/* 固定检测配置里指定的channel，简化代码，如改变需修改结构体 */
typedef struct cc85xx_nwm_get_status_s
{
    CC85XX_DEV_INFO_S   nwm_dev_info;
    uint8_t             byte12_to_20[9];
    ACH_INFO_S          mysel_ach;
    uint8_t             byte22_to_29[8];
} CC85XX_NWM_GET_STATUS_S;

int ehif_DI_GET_DEVICE_INFO(CC85XX_DEV_INFO_S *dev_info);
int ehif_PM_SET_STATE(uint8_t pwr_state);
int ehif_VC_SET_VOLUME(uint8_t mute_ops, uint8_t set_ops, uint16_t volume);
int ehif_VC_GET_VOLUME(uint16_t *volume);

int ehif_NVS_GET_DATA(uint8_t slot, uint32_t *nwm_id);
int ehif_NVS_SET_DATA(uint8_t slot, uint32_t nwm_id);

int ehif_NWM_DO_JOIN(uint16_t join_to, uint32_t device_id, uint32_t manf_id);
int ehif_NWM_GET_STATUS(CC85XX_NWM_GET_STATUS_S *nwm_status);
int ehif_NWM_ACH_SET_USAGE(uint8_t single_chnn);

int ehif_RC_SET_DATA(uint8_t *data, uint8_t count);



#endif /* CC85XX_EHIF_HOST_CTRL_H */
