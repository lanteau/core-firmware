#include "outlet_control.h"
#include "stm32f4xx_gpio.h"


GPIO_TypeDef* RELAY_GPIO_PORT[] = {RELAY_1_PORT, RELAY_2_PORT, RELAY_3_PORT,
                                RELAY_4_PORT, RELAY_5_PORT, RELAY_6_PORT};
const uint16_t RELAY_GPIO_PIN[] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN,
                                RELAY_4_PIN, RELAY_5_PIN, RELAY_6_PIN};
const uint32_t RELAY_GPIO_CLK[] = {RELAY_1_CLK, RELAY_2_CLK, RELAY_3_CLK,
                                RELAY_4_CLK, RELAY_5_CLK, RELAY_6_CLK};    

void Relays_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the GPIO Clock */
    RCC_AHB1PeriphClockCmd(RELAY_GPIO_CLK[RELAY1]   |
                            RELAY_GPIO_CLK[RELAY2]  |
                            RELAY_GPIO_CLK[RELAY3]  |
                            RELAY_GPIO_CLK[RELAY4]  |
                            RELAY_GPIO_CLK[RELAY5]  |
                            RELAY_GPIO_CLK[RELAY6], ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    int i;
    for(i = 0; i < RELAYn; i++)
    {
        GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN[i];
        GPIO_Init(RELAY_GPIO_PORT[i], &GPIO_InitStructure);
    }
}

void outlet_set_state(Relay_TypeDef outlet, Outlet_State_TypeDef NewState)
{
	if(NewState == ON)
	{
		RELAY_GPIO_PORT[outlet]->BSRRL = RELAY_GPIO_PIN[outlet];
	}
	else
	{
		RELAY_GPIO_PORT[outlet]->BSRRH = RELAY_GPIO_PIN[outlet];
	}

}