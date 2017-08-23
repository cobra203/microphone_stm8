#include <stm8s.h>
#include <stm8s_tim4.h>
#include <stm8s_clk.h>

#include <stm8s_timer.h>
#include <stm8s_common.h>
#include <stm8s_debug.h>

TIMER_OBJ_S basic_timer = {0};

int timer_alloc(uint8_t *timer)
{
    uint8_t i;
    uint8_t active = basic_timer.active;

    for(i = 0; i < TIMERS_NUM; i++, active >>= 1) {
        if(!(active & 0x1)) {
            BIT_SET(basic_timer.active, i);
            *timer = i;
            return 0;
        }
    }

    return -1;
}

void timer_free(uint8_t *timer)
{
    BIT_CLR(basic_timer.active, *timer);
    BIT_CLR(basic_timer.touch, *timer);
    basic_timer.delay_count[*timer] = 0;
    basic_timer.callback_func[*timer] = 0;
    *timer = TIMERS_NUM;
    
}

void timer_set_reload(uint8_t id, uint16_t reload)
{
    basic_timer.delay_reload[id] = reload;
}

static void timer_touch_process(TIMER_OBJ_S *timer, uint8_t idx)
{
    TASK_F      callback = 0;
    void        *callback_args = 0;

    if(!timer->callback_func[idx]) {
        return;
    }
    
    switch(timer->type[idx]) {
    case TMR_ONCE:
        callback        = timer->callback_func[idx];
        callback_args   = timer->callback_args[idx];
        timer_free(&idx);
        callback(callback_args);
        break;
    case TMR_CYCLICITY:
        BIT_CLR(timer->touch, idx);
        timer->delay_count[idx] = timer->delay_reload[idx];
        timer->callback_func[idx](timer->callback_args[idx]);
        break;
    default:
        timer_free(&idx);
        break;
    }
}

void timer_itc(void)
{
    uint8_t     i;
    uint8_t     active = basic_timer.active;
    
    if(TIM4_GetITStatus(TIM4_IT_UPDATE) != RESET) {
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    }
    else {
        return;
    }

    for(i = 0; i < TIMERS_NUM; i++, active >>= 1) {
        if(active & 0x1) {
            if(basic_timer.delay_count[i]) {
                basic_timer.delay_count[i]--;
                if(!basic_timer.delay_count[i]) {
                    BIT_SET(basic_timer.touch, i);
                    timer_touch_process(&basic_timer, i);
                    
                }
            }
        }
    }
}

void timer_init(void) /* 1ms timer */
{
    disableInterrupts();

    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);

    enableInterrupts();
}

void delay_ms(uint16_t time)   
{
    uint8_t timer;

    timer_task(&timer, TMR_ONCE, time, 0, 0, (void *)0);
    
    while(basic_timer.delay_count[timer]);

    timer_free(&timer);
}

void timer_task(uint8_t *timer, TIMER_TYPE_E type, uint16_t delay, uint16_t load, TASK_F task, void *args)
{
    while(timer_alloc(timer));
    BIT_CLR(basic_timer.touch, *timer);

    basic_timer.type[*timer]            = type;
    basic_timer.callback_func[*timer]   = task;
    basic_timer.callback_args[*timer]   = args;
    basic_timer.delay_reload[*timer]    = load;
    basic_timer.delay_count[*timer]     = delay;
}


