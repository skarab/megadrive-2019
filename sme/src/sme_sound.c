#include "sme_sound.h"

u32 smeSOUND_GetTime(u32 pcm)
{
    Z80_requestBus(1);
    vu8 *pb = (u8*)(Z80_DRV_PARAMS+0x10);
    u8 a=pb[0];
    u8 b=pb[1];
    Z80_releaseBus();
    u32 cr = (u32)a<<8|(u32)b<<16;
    cr -= pcm;
    cr /= 16;
    return cr;
}
