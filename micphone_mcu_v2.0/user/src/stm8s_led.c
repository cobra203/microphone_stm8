#include <stm8s_gpio.h>
#include <stm8s_debug.h>

#include <stm8s_led.h>
#include <stm8s_timer.h>

static void _led_bright(STM8S_LED_S *led)
{
    GPIO_WriteLow(led->port, (GPIO_Pin_TypeDef)led->pin);
    led->status = 1;
}

static void _led_dark(STM8S_LED_S *led)
{
    GPIO_WriteHigh(led->port, (GPIO_Pin_TypeDef)led->pin);
    led->status = 0;
}

static void _led_set_attr(STM8S_LED_S *led, uint16_t dark_time, uint16_t bright_time)
{
    led->interval[0]    = dark_time;
    led->interval[1]    = bright_time;
}

static void _led_callback_func(void *args)
{
    STM8S_LED_S *led = (STM8S_LED_S *)args;

    if(!led->status) {
        led->bright(led);
        timer_set_reload(led->task, led->interval[0]);
    }
    else {
        led->dark(led);
        timer_set_reload(led->task, led->interval[1]);
    }
}

static void _led_doing(STM8S_LED_S *led)
{
    if(led->task < TIMERS_NUM) {
        timer_free(&led->task);
        led->dark(led);
    }

    if(!led->interval[1]) {
        led->dark(led);
    }
    else if(!led->interval[0]) {
        led->bright(led);
    }
    else {
        timer_task(&led->task, TMR_CYCLICITY, led->interval[0], led->interval[1], _led_callback_func, (void *)led);
    }
}

void stm8s_led_init(STM8S_LED_S *led)
{
    led->port   = GPIOC;
    led->pin    = GPIO_PIN_4;

    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);

    led->set    = _led_set_attr;
    led->doing  = _led_doing;
    led->dark   = _led_dark;
    led->bright = _led_bright;

    led->task   = TIMERS_NUM;
    led->status = 0;
}


