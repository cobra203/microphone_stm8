#include <mic_led.h>

static STM8S_LED_S led;

void led_init(void)
{
    stm8s_led_init(&led);
}

void mic_led_dark(void)
{
    led.set(&led, 1, 0);
    led.doing(&led);
}

void mic_led_bright(void)
{
    led.set(&led, 0, 1);
    led.doing(&led);
}

void mic_led_flicker(void)
{
    led.set(&led, 180, 20);
    led.doing(&led);
}

void mic_led_burn(void)
{
    led.set(&led, 480, 20);
    led.doing(&led);
}




