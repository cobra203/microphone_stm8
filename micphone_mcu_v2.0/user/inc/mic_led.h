#ifndef _MIC_LED_H_
#define _MIC_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm8s_led.h>

void led_init(void);
void mic_led_dark(void);
void mic_led_bright(void);
void mic_led_flicker(void);
void mic_led_burn(void);


#ifdef __cplusplus
}
#endif

#endif /* _MIC_LED_H_ */

