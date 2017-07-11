#include <stm8s.h>
#include <stm8s_tim4.h>
#include <stm8s_clk.h>

#include <stm8s_timer.h>
#include <stm8s_common.h>
#include <stm8s_debug.h>

TIMER_OBJ_S basic_timer = {0};

static int timer_alloc(uint8_t *timer)
{
    uint8_t i;
    uint16_t status = basic_timer.timer_status;

    for(i = 0; i < sizeof(status) * 8; i++, status >>= 1) {
        if(!(status & 0x1)) {
            BIT_SET(basic_timer.timer_status, i);
            *timer = i;
            return 0;
        }
    }

    return -1;
}

static void timer_free(uint8_t timer)
{
    BIT_CLR(basic_timer.timer_status, timer);
    BIT_CLR(basic_timer.touch, timer);
    basic_timer.delay_count[timer] = 0;
    basic_timer.callback[timer] = 0;   
}

void timer_init(void) /* 1ms timer */
{
    disableInterrupts();

    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);

    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);

    enableInterrupts();
}

void timer_touch_process(void)
{
    uint8_t     i;
    uint16_t    status = basic_timer.timer_status;

    for(i = 0; i < sizeof(status) * 8; i++, status >>= 1) {
        if(status & 0x1) {
            if(BIT_ISSET(basic_timer.touch, i)) {
                basic_timer.callback[i](basic_timer.callback_args[i]);
                timer_free(i);
            }
        }
    }
}

void delay_ms(uint32_t time)   
{
    uint8_t timer;
    
    while(timer_alloc(&timer));
    basic_timer.delay_count[timer] = time;

    while(basic_timer.delay_count[timer]);

    timer_free(timer);
}

void timer_task_once(uint32_t time, TASK_F task, void *args)
{
    uint8_t timer;

    while(timer_alloc(&timer));
    BIT_CLR(basic_timer.touch, timer);
    basic_timer.callback[timer]         = task;
    basic_timer.callback_args[timer]    = args;
    basic_timer.delay_count[timer]      = time;
}



