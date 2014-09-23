#ifndef __OUTLET_CONTROL_H
#define __OUTLET_CONTROL_H
#include "hw_config.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef enum
{
	OFF = 0, ON = 1
} Outlet_State_TypeDef;

typedef enum 
{
  RELAY1 = 0, RELAY2 = 1, RELAY3 = 2, RELAY4 = 3, RELAY5 = 4, RELAY6 = 5
} Relay_TypeDef;

#define RELAYn        6

void Relays_Init();
void outlet_set_state(Relay_TypeDef outlet, Outlet_State_TypeDef NewState);

#if defined (__cplusplus)
}
#endif

#endif