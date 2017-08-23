/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include <stm8s_spi.h>
#include <stm8s_debug.h>

#include <cc85xx_host_ops.h>
#include <cc85xx_button.h>
#include <stm8s_timer.h>
#include <stm8s_led.h>
#include <stm8s_wwdg.h>
#include <mic_led.h>

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int burn_init(void)
{
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
    mic_led_burn();
    
    return 0;
}

int burn_detect(void)
{
    uint8_t port_value;
    port_value = GPIO_ReadInputData(GPIOB);

    return (~port_value >> 5) & 0x1;
}

static void clk_init(void)
{
    disableInterrupts();

    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);

    enableInterrupts();
}

void main(void)
{
    //while(1);

    /* init */
    clk_init();
    timer_init();
    led_init();
    burn_init();

    while(burn_detect()) {
        delay_ms(2000);
    }

    host_init();
    button_init();

    /* Infinite loop */
    while (1)
    {
        delay_ms(10);

        if(burn_detect()) {
            WWDG_SWReset();
        }

        button_detect_input();
        button_updata_status();
        button_precess();

        host_ehif_updata_status();
        host_precess();
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number  */
    DEBUG("Wrong parameters value: file %s on line %d\n", file, (int)line);

  /* Infinite loop */
    while (1)
    {
    }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
