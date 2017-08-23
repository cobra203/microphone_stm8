#include <string.h>
#include <stm8s.h>
#include <stm8s_gpio.h>
#include <cc85xx_host_ops.h>
#include <stm8s_debug.h>
#include <stm8s_common.h>
#include <stm8s_timer.h>
#include <button.h>
#include <mic_led.h>

typedef enum button_id_e
{
    BTN_ID_PC3_PAIRING = 0,
    BTN_ID_PD2_MUTE_5,          //mute
    BTN_ID_PD3_FORWARD_4,       //next
    BTN_ID_PD4_VOLUP_1,         //up
    BTN_ID_PD5_BACK_2,          //back
    BTN_ID_PD6_VOLDOWN_3,       //down
    BTN_ID_BUTT,
} BUTTON_ID_E;

#define     BUTTON_GPIO_PINS  (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)
#define     BUTTON_NUM  BTN_ID_BUTT

static BUTTON_S button[BUTTON_NUM];

int _button_process(struct button_s *button)
{
    REMOTE_CONTROL_CMD_E    cmd;
    uint8_t                 timer;

    if(button->state.avtice) {
        button->state.avtice = 0;
        DEBUG("B %d\n", button->type);
        switch(button->type) {
        case BTN_ID_PC3_PAIRING:
            host_pairing_network(1000);
            mic_led_flicker();
            break;
        case BTN_ID_PD2_MUTE_5:            
            if(button->state.effective != ECT_FOCUSED) {
                cmd = INPUT_VOLUME_MUTE;
                host_remote_control_send(cmd);
                timer_task(&timer, TMR_ONCE, 100, 0, host_remote_control_send_stop, (void *)&cmd);
            }
            break;
        case BTN_ID_PD3_FORWARD_4:
            //mic_led_dark();
            break;
        case BTN_ID_PD4_VOLUP_1:
            cmd = OUTPUT_VOLUME_INCREMENT;
            host_remote_control_send(cmd);         
            timer_task(&timer, TMR_ONCE, 100, 0, host_remote_control_send_stop, (void *)&cmd);
            break;
        case BTN_ID_PD5_BACK_2:
            //mic_led_bright();
            break;
        case BTN_ID_PD6_VOLDOWN_3:
            cmd = OUTPUT_VOLUME_DECREASE;
            host_remote_control_send(cmd);         
            timer_task(&timer, TMR_ONCE, 100, 0, host_remote_control_send_stop, (void *)&cmd);
            break;
        }
    }

    return 0;
}

void button_init(void)
{
    int i = 0;
    
    GPIO_Init(GPIOD, BUTTON_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
    
    for(i = 0; i < BTN_ID_BUTT; i++) {
        button[i].check_active      = button_check_active;
        button[i].process           = _button_process;
        button[i].type              = i;
        button[i].interval.shack    = 3;
        button[i].interval.pressed  = 20;
        button[i].interval.focused  = 100;
    }
}

void button_detect_input(void)
{
    uint8_t portd_value = 0;
    uint8_t portc_value = 0;
    int     i = 0;
    
    portd_value = GPIO_ReadInputData(GPIOD);
    portc_value = GPIO_ReadInputData(GPIOC);
    
    for(i = 0; i < BTN_ID_BUTT; i++) {
        if(!i) {
            button[i].state.press = (~portc_value >> 3) & 0x1;
        }
        else {
            button[i].state.press = (~portd_value >> (1 + i)) & 0x1;
        }

    }
}

void button_updata_status(void)
{
    int i = 0;

    for(i = 0; i < BTN_ID_BUTT; i++) {
        button[i].check_active(&button[i]);
    }
}

void button_precess(void)
{
    int i = 0;

    for(i = 0; i < BTN_ID_BUTT; i++) {
        button[i].process(&button[i]);
    }
}
