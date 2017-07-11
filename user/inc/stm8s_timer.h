#ifndef STM8S_TIMER_H
#define STM8S_TIMER_H
#include <stm8s.h>

typedef void (*TASK_F) (void *);

typedef struct timer_obj_s
{
    uint16_t            timer_status;
    uint16_t            touch;
    volatile uint32_t   delay_count[sizeof(timer_status) * 8];
    TASK_F              callback[sizeof(timer_status) * 8];
    void                *callback_args[sizeof(timer_status) * 8];
} TIMER_OBJ_S;

extern TIMER_OBJ_S basic_timer;

void timer_init(void);
void timer_touch_process(void);
void timer_task_once(uint32_t time, TASK_F task, void *args);
void delay_ms(uint32_t time);

#endif /* STM8S_TIMER_H */
