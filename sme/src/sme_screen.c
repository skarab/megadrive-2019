#include "sme_screen.h"

void smeSCREEN_SetScrollMode(u8 horizontal, u8 vertical)
{
    VDP_setReg(0x0b, horizontal|vertical);
} 
