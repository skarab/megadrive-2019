#include "sme_framebuffer.h"
#include "sme_screen.h"

u16* smeFRAMEBUFFER;
const u16 smeFRAMEBUFFER_Width = 64;    // 64*4 -> 256
const u16 smeFRAMEBUFFER_Height = 160;
u16* smeFRAMEBUFFER_Offsets;

void smeFRAMEBUFFER_Create()
{
    smeFRAMEBUFFER = NULL;
    
    smeFRAMEBUFFER_Offsets = (u16*)MEM_alloc(smeFRAMEBUFFER_Height*sizeof(u16));
    u16 i;
    for (i=0 ; i<smeFRAMEBUFFER_Height ; ++i)
        smeFRAMEBUFFER_Offsets[i] = i*smeFRAMEBUFFER_Width;
}

void smeFRAMEBUFFER_Destroy()
{
    MEM_free(smeFRAMEBUFFER_Offsets);
}

void smeFRAMEBUFFER_Begin()
{
    BMP_init(0, 0, 0);
    smeSCREEN_SetScrollMode(smeSCREEN_HSCROLL_Plane, smeSCREEN_VSCROLL_Plane);
    smeFRAMEBUFFER = (u16*)BMP_getWritePointer(0, 0);
    
    M3D_reset();
    M3D_setViewport(smeFRAMEBUFFER_Width, smeFRAMEBUFFER_Height);
}

void smeFRAMEBUFFER_End()
{
    BMP_end();
}

void smeFRAMEBUFFER_Update()
{
    BMP_flip(1);
    BMP_clear();
    smeFRAMEBUFFER = (u16*)BMP_getWritePointer(0, 0);
}
