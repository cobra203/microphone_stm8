#ifndef _STM8S_LED_H_
#define _STM8S_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm8s.h>

typedef struct stm8s_led_s
{
    GPIO_TypeDef    *port;
    uint8_t         pin;
    uint8_t         task;
    uint16_t        interval[2];
    uint8_t         status;
    void            (*init)         (struct stm8s_led_s *, uint16_t);
    void            (*set)          (struct stm8s_led_s *, uint16_t, uint16_t);
    void            (*doing)        (struct stm8s_led_s *);
    void            (*dark)         (struct stm8s_led_s *);
    void            (*bright)       (struct stm8s_led_s *);
} STM8S_LED_S;

void stm8s_led_init(STM8S_LED_S *led);

#ifdef __cplusplus
}
#endif

#endif /* _STM8S_LED_H_ */

