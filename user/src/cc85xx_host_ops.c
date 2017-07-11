#include <stm8s.h>

#include <cc85xx_host_ops.h>
#include <cc85xx_ehif.h>
#include <cc85xx_ehif_host_ctrl.h>

#include <stm8s_timer.h>
#include <stm8s_debug.h>

#define MICPHONE_MANF_ID    0x00000001

typedef enum cc85xx_usage_e
{
    USAGE_MICROPHONE_0 = 12,
    USAGE_MICROPHONE_1 = 13,
    USAGE_BUTT         = 16,
} CC85XX_USAGE_E;

typedef struct nwm_info_s
{
    uint32_t    device_id;
    uint8_t     free_chnn;
} NWM_INFO_S;

static SPI_STATUS_S ehif_status;
static uint32_t     default_network;
static uint32_t     net_con_flag;

static void _host_ehif_wait_ready(void)
{
    basic_GET_STATUS(&ehif_status);
    while(ehif_status.cmdreq_rdy == 0) {
        delay_ms(10);
        basic_GET_STATUS(&ehif_status);
    }
}

static inline void _host_init_default_network(void)
{
    ehif_NVS_GET_DATA(0, &default_network);
    DEBUG("DNID:0x%08lx\n", default_network);
}

static void _host_get_info_from_nwm_status(NWM_INFO_S *nwm_info)
{
    CC85XX_NWM_GET_STATUS_S nwm_status;

    nwm_info->device_id = 0;
    nwm_info->free_chnn = USAGE_BUTT;

    _host_ehif_wait_ready();
    if(!ehif_NWM_GET_STATUS(&nwm_status)) {
        nwm_info->device_id = nwm_status.nwm_dev_info.device_id;

        if(!nwm_status.mysel_ach.active_a) {
            nwm_info->free_chnn = USAGE_MICROPHONE_0;
        }
        else if(!nwm_status.mysel_ach.active_b) {
            nwm_info->free_chnn = USAGE_MICROPHONE_1;
        }
    }
}

static void _host_updata_default_network(uint32_t network_id)
{
    if(network_id != default_network) {
        ehif_NVS_SET_DATA(0, network_id);
        default_network = network_id;
        DEBUG("UP:NID:0x%08lx\n", default_network);
    }
}

static void _host_set_audio_channel(uint8_t single_chnn)
{
    if(single_chnn < USAGE_BUTT) {
        ehif_NWM_ACH_SET_USAGE(single_chnn);
    }
}

static int _host_try_join_default_network(uint16_t join_to)
{
    if(ehif_status.cmdreq_rdy) {
        ehif_NWM_DO_JOIN(join_to, default_network, MICPHONE_MANF_ID);
        return 0;
    }
    return -1;
}

void host_init(void)
{
    spi_init();
    basic_SYS_RESET();
    _host_ehif_wait_ready();
    _host_init_default_network();
    _host_try_join_default_network(100);
}

void host_ehif_updata_status(void)
{
    basic_GET_STATUS(&ehif_status);
}

void host_precess(void)
{
    NWM_INFO_S nwm_info;

    if(ehif_status.wasp_conn) {
        if(!net_con_flag) {
            DEBUG("Cn\n");
            _host_get_info_from_nwm_status(&nwm_info);
            _host_updata_default_network(nwm_info.device_id);
            _host_set_audio_channel(nwm_info.free_chnn);      
            net_con_flag = 1;
        }
    }
    else {
        if(net_con_flag) {
            net_con_flag = 0;
            delay_ms(300);
            host_ehif_updata_status();
        }
        _host_try_join_default_network(100);
    }
}

void host_pairing_network(uint16_t join_to)
{
    _host_ehif_wait_ready();
    ehif_NWM_DO_JOIN(join_to, 0xffffffff, MICPHONE_MANF_ID);
}

void host_remote_control_send(REMOTE_CONTROL_CMD_E cmd)
{
    uint8_t rc_cmd = cmd;
    ehif_RC_SET_DATA(&rc_cmd, sizeof(uint8_t));
}

void host_remote_control_send_stop(void *cmd)
{
    uint8_t rc_cmd = *(REMOTE_CONTROL_CMD_E *)cmd;
    ehif_RC_SET_DATA(&rc_cmd, 0);
}


