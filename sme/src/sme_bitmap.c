#include "sme_bitmap.h"

void smeBITMAP_acquire(smeBitmap* bitmap)
{
    smeBITMAP_acquire_buffer(bitmap, (u8*)MEM_alloc(bitmap->Depth*2+bitmap->Width*bitmap->Height*2));
}

void smeBITMAP_acquire_buffer(smeBitmap* bitmap, u8* buffer)
{
    decompress_slz(buffer, bitmap->Packed);
    bitmap->Palette = (u16*)buffer;
    bitmap->Image = (u16*)(buffer+bitmap->Depth*2);
    bitmap->Unpacked = buffer;        
}

void smeBITMAP_release(smeBitmap* bitmap)
{
    MEM_free(bitmap->Unpacked);
    smeBITMAP_release_buffer(bitmap);
}

void smeBITMAP_release_buffer(smeBitmap* bitmap)
{
    bitmap->Unpacked = NULL;
    bitmap->Image = NULL;
    bitmap->Palette = NULL;
}
