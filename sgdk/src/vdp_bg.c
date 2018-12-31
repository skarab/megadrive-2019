#include "config.h"
#include "types.h"

#include "vdp.h"
#include "vdp_bg.h"

#include "tools.h"
#include "string.h"
#include "vdp_dma.h"
#include "vdp_pal.h"
#include "vdp_tile.h"

#include "memory.h"

void VDP_setHorizontalScroll(VDPPlan plan, s16 value)
{
    vu16 *pw;
    vu32 *pl;
    u16 addr;

    /* Point to vdp port */
    pw = (u16 *) GFX_DATA_PORT;
    pl = (u32 *) GFX_CTRL_PORT;

    addr = HSCRL;
    if (plan.v == PLAN_B.v) addr += 2;

    *pl = GFX_WRITE_VRAM_ADDR(addr);
    *pw = value;
}

void VDP_setHorizontalScrollTile(VDPPlan plan, u16 tile, s16* values, u16 len, u16 use_dma)
{
    u16 addr;

    addr = HSCRL + ((tile & 0x1F) * (4 * 8));
    if (plan.v == PLAN_B.v) addr += 2;

    VDP_setAutoInc(4 * 8);

    if (use_dma) VDP_doDMAEx(VDP_DMA_VRAM, (u32) values, addr, len, -1);
    else
    {
        vu16 *pw;
        vu32 *pl;
        u16 *src;
        u16 i;

        /* Point to vdp port */
        pw = (u16 *) GFX_DATA_PORT;
        pl = (u32 *) GFX_CTRL_PORT;

        *pl = GFX_WRITE_VRAM_ADDR(addr);

        src = values;

        i = len;
        while(i--) *pw = *src++;
    }
}

void VDP_setHorizontalScrollLine(VDPPlan plan, u16 line, s16* values, u16 len, u16 use_dma)
{
    u16 addr;

    addr = HSCRL + ((line & 0xFF) * 4);
    if (plan.v == PLAN_B.v) addr += 2;

    VDP_setAutoInc(4);

    if (use_dma) VDP_doDMAEx(VDP_DMA_VRAM, (u32) values, addr, len, -1);
    else
    {
        vu16 *pw;
        vu32 *pl;
        u16 *src;
        u16 i;

        /* Point to vdp port */
        pw = (u16 *) GFX_DATA_PORT;
        pl = (u32 *) GFX_CTRL_PORT;

        *pl = GFX_WRITE_VRAM_ADDR(addr);

        src = values;

        i = len;
        while(i--) *pw = *src++;
    }
}

void VDP_setVerticalScroll(VDPPlan plan, s16 value)
{
    vu16 *pw;
    vu32 *pl;
    u16 addr;

    /* Point to vdp port */
    pw = (u16 *) GFX_DATA_PORT;
    pl = (u32 *) GFX_CTRL_PORT;

    addr = 0;
    if (plan.v == PLAN_B.v) addr += 2;

    *pl = GFX_WRITE_VSRAM_ADDR(addr);
    *pw = value;
}

void VDP_setVerticalScrollTile(VDPPlan plan, u16 tile, s16* values, u16 len, u16 use_dma)
{
    u16 addr;

    addr = (tile & 0x1F) * 4;
    if (plan.v == PLAN_B.v) addr += 2;

    VDP_setAutoInc(4);

    if (use_dma) VDP_doDMAEx(VDP_DMA_VSRAM, (u32) values, addr, len, -1);
    else
    {
        vu16 *pw;
        vu32 *pl;
        u16 *src;
        u16 i;

        /* Point to vdp port */
        pw = (u16 *) GFX_DATA_PORT;
        pl = (u32 *) GFX_CTRL_PORT;

        *pl = GFX_WRITE_VSRAM_ADDR(addr);

        src = values;

        i = len;
        while(i--) *pw = *src++;
    }
}


void VDP_clearPlan(u16 plan, u8 use_dma)
{
    if (use_dma)
    {
        // wait for previous DMA completion
        VDP_waitDMACompletion();
        // then do DMA
        VDP_doVRamDMAFill(plan, VDP_getPlanWidth() * VDP_getPlanHeight() * 2, 0);
    }
    else
    {
        vu32 *plctrl;
        vu32 *pldata;
        u16 i;

        /* point to vdp port */
        plctrl = (u32 *) GFX_CTRL_PORT;
        pldata = (u32 *) GFX_DATA_PORT;

        *plctrl = GFX_WRITE_VRAM_ADDR(plan);

        // unroll a bit
        i = VDP_getPlanWidth() * VDP_getPlanHeight() / (2 * 8);
        while (i--)
        {
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
            *pldata = 0;
        }
    }
}



u16 VDP_drawBitmap(u16 plan, const Bitmap *bitmap, u16 x, u16 y)
{
    return VDP_drawBitmapEx(plan, bitmap, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USERINDEX), x, y, TRUE);
}

u16 VDP_drawBitmapEx(u16 plan, const Bitmap *bitmap, u16 basetile, u16 x, u16 y, u16 loadpal)
{
    const int wt = bitmap->w / 8;
    const int ht = bitmap->h / 8;
    const Palette *palette = bitmap->palette;

    // compressed bitmap ?
    if (bitmap->compression != COMPRESSION_NONE)
    {
        Bitmap *b = unpackBitmap(bitmap, NULL);

        if (b == NULL) return FALSE;

        // tiles
        VDP_loadBMPTileData((u32*) b->image, basetile & TILE_INDEX_MASK, wt, ht, wt);
        MEM_free(b);
    }
    else
        // tiles
        VDP_loadBMPTileData((u32*) bitmap->image, basetile & TILE_INDEX_MASK, wt, ht, wt);

    // tilemap
    VDP_fillTileMapRectInc(plan, basetile, x, y, wt, ht);
    // palette
    if (loadpal) VDP_setPaletteColors(((basetile >> 9) & 0x30) + (palette->index & 0xF), palette->data, palette->length);

    return TRUE;
}

u16 VDP_drawImage(u16 plan, const Image *image, u16 x, u16 y)
{
    return VDP_drawImageEx(plan, image, TILE_ATTR_FULL(PAL0, 0, 0, 0, TILE_USERINDEX), x, y, TRUE, TRUE);
}

u16 VDP_drawImageEx(u16 plan, const Image *image, u16 basetile, u16 x, u16 y, u16 loadpal, u16 use_dma)
{
    Palette *palette;

    if (!VDP_loadTileSet(image->tileset, basetile & TILE_INDEX_MASK, use_dma))
        return FALSE;

    if (!VDP_setMap(plan, image->map, basetile, x, y))
        return FALSE;

    palette = image->palette;

    // palette
    if (loadpal) VDP_setPaletteColors(((basetile >> 9) & 0x30) + (palette->index & 0xF), palette->data, palette->length);

    return TRUE;
}
