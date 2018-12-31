#ifndef _SME_FRAMEBUFFER_H_
#define _SME_FRAMEBUFFER_H_

#include "genesis.h"

extern u16* smeFRAMEBUFFER;
extern const u16 smeFRAMEBUFFER_Width;
extern const u16 smeFRAMEBUFFER_Height;
extern u16* smeFRAMEBUFFER_Offsets;

void smeFRAMEBUFFER_Create();
void smeFRAMEBUFFER_Destroy();
void smeFRAMEBUFFER_Begin();
void smeFRAMEBUFFER_End();
void smeFRAMEBUFFER_Update();

#endif
