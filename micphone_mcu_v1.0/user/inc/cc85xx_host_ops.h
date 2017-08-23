#ifndef CC85XX_HOST_OPS_H
#define CC85XX_HOST_OPS_H

#include <stm8s.h>

typedef enum remote_control_cmd_e
{
    OUTPUT_VOLUME_MUTE      = 1,
    OUTPUT_VOLUME_INCREMENT = 2,
    OUTPUT_VOLUME_DECREASE  = 3,
    INPUT_VOLUME_MUTE       = 4,
    NETWORK_STANDBY_ENABLE  = 5,
    NETWORK_STANDBY_DISABLE = 6,
    NETWORK_STANDBY_TOGGLE  = 7,
    PLAY_PAUSE              = 16,
    SCAN_NEXT_TRACK         = 17,
    SCAN_PREVIOUS_TRACK     = 18,
    FAST_FORWARD            = 19,
    REWIND                  = 20,
} REMOTE_CONTROL_CMD_E;

void host_init(void);
void host_ehif_updata_status(void);
void host_pairing_network(uint16_t join_to);
void host_precess(void);
void host_remote_control_send(REMOTE_CONTROL_CMD_E cmd);
void host_remote_control_send_stop(void *cmd);



#endif /* CC85XX_HOST_OPS_H */
