#ifndef STM8S_TIMER_H
#define STM8S_TIMER_H
#include <stm8s.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef void (*TASK_F) (void *);

typedef enum timer_type_e
{
    TMR_ONCE,
    TMR_CYCLICITY,
} TIMER_TYPE_E;

#define TIMERS_NUM (sizeof(uint8_t) * 4)

typedef struct timer_obj_s
{
    uint8_t             active;
    uint8_t             touch;
    TIMER_TYPE_E        type[TIMERS_NUM];
    uint16_t            delay_reload[TIMERS_NUM];
    volatile uint16_t   delay_count[TIMERS_NUM];
    TASK_F              callback_func[TIMERS_NUM];
    void                *callback_args[TIMERS_NUM];
} TIMER_OBJ_S;

int  timer_alloc(uint8_t *timer);
void timer_free(uint8_t *timer);
void timer_set_reload(uint8_t id, uint16_t reload);
void timer_itc(void);
void timer_init(void);
void timer_task(uint8_t *timer, TIMER_TYPE_E type, uint16_t delay, uint16_t load, TASK_F task, void *args);
void delay_ms(uint16_t time);

#ifdef __cplusplus
}
#endif

#endif /* STM8S_TIMER_H */
