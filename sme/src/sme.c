#include "sme.h"

void sme_Init(u16 hard)
{
    if (hard==0)
    {
        // handle reset
    }
    
    VDP_init();
    VDP_setHInterrupt(0);
    VDP_setHilightShadow(0);
    VDP_setScreenWidth320(); 

    smeFRAMEBUFFER_Create();    
    smeSCENE_Create();
}

void sme_Exit()
{
    smeSCENE_Destroy();
    smeFRAMEBUFFER_Destroy();
}

