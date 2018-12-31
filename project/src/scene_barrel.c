#include "scene_barrel.h"

int SCENE_Barrel_Depth = 16;
int SCENE_Barrel_Height;
int* SCENE_Barrel_Scanlines;
int* SCENE_Barrel_DepthBuffer;
int* SCENE_Barrel_FrameBuffers[2];
smeMesh* SCENE_Barrel_Mesh;
int SCENE_Barrel_BitmapWidth;
int SCENE_Barrel_BitmapHeight;

void SCENE_Barrel_Create()
{
    SCENE_Barrel_Height = 32;
    SCENE_Barrel_Scanlines = (int*)MEM_alloc(SCENE_Barrel_Depth*SCENE_Barrel_Height*sizeof(int));
    
    SCENE_Barrel_BitmapWidth = barrel_bump.w;
    SCENE_Barrel_BitmapHeight = barrel_bump.h;
    
    int width = SCENE_Barrel_BitmapWidth/2;
    int depth = SCENE_Barrel_Depth-1;
    int offset = 0;
    int x, y;
    while (depth>=0)
    {
        int height = width*SCENE_Barrel_Height/(SCENE_Barrel_BitmapWidth/2);
        for (y=0 ; y<SCENE_Barrel_Height ; ++y)
        {
            if (depth<4)
            {
                SCENE_Barrel_Scanlines[depth*SCENE_Barrel_Height+y] = SCENE_Barrel_BitmapHeight;
            }
            else
            {
                SCENE_Barrel_Scanlines[depth*SCENE_Barrel_Height+y] = offset+y*height/SCENE_Barrel_Height;
            }
        }
        offset += height;
        width -= 2;
        --depth;
    }
    
    SCENE_Barrel_DepthBuffer = (int*)MEM_alloc(screenHeight*sizeof(int));
    SCENE_Barrel_FrameBuffers[0] = (int*)MEM_alloc(screenHeight*sizeof(int));
    SCENE_Barrel_FrameBuffers[1] = (int*)MEM_alloc(screenHeight*sizeof(int));
    
    SCENE_Barrel_Mesh = smeMESH_Create();
    SCENE_Barrel_Mesh->VertexCount = 4;
    SCENE_Barrel_Mesh->Vertexes = (Vect3D_f16*)MEM_alloc(SCENE_Barrel_Mesh->VertexCount*sizeof(Vect3D_f16));
    SCENE_Barrel_Mesh->Vertexes[0].x = intToFix16(0); SCENE_Barrel_Mesh->Vertexes[0].y = intToFix16(-14); SCENE_Barrel_Mesh->Vertexes[0].z = intToFix16(-14);
    SCENE_Barrel_Mesh->Vertexes[1].x = intToFix16(0); SCENE_Barrel_Mesh->Vertexes[1].y = intToFix16(-14); SCENE_Barrel_Mesh->Vertexes[1].z = intToFix16(14);
    SCENE_Barrel_Mesh->Vertexes[2].x = intToFix16(0); SCENE_Barrel_Mesh->Vertexes[2].y = intToFix16(14); SCENE_Barrel_Mesh->Vertexes[2].z = intToFix16(14);
    SCENE_Barrel_Mesh->Vertexes[3].x = intToFix16(0); SCENE_Barrel_Mesh->Vertexes[3].y = intToFix16(14); SCENE_Barrel_Mesh->Vertexes[3].z = intToFix16(-14);
}

void SCENE_Barrel_Destroy()
{
    smeMESH_Destroy(SCENE_Barrel_Mesh);
    MEM_free(SCENE_Barrel_FrameBuffers[1]);
    MEM_free(SCENE_Barrel_FrameBuffers[0]);
    MEM_free(SCENE_Barrel_DepthBuffer);
    MEM_free(SCENE_Barrel_Scanlines);
}

void SCENE_Barrel()
{
    s16* hTable;
    s16* hTable2;
    
    u16 curY;
    u16 ttt=0;
    
    SCENE_Barrel_Create();
    
    void initTable()
    {
        hTable = (s16*)MEM_alloc(screenHeight*sizeof(s16));
        hTable2 = (s16*)MEM_alloc(screenHeight*sizeof(s16));
        
        int h;
        int o = 96;
        for (h=0 ; h<screenHeight ; ++h)
        {
            if (h<screenHeight/2)
            {
                hTable[h] = o;
            }
            else
            {
                hTable[h] = o-SCENE_Barrel_BitmapWidth/2;
            }
            
            hTable2[h] = 0;
        }
        
        M3D_reset();
        M3D_setViewport(screenWidth, screenHeight);
    
        VDP_setPaletteColors(0, barrel_bump.palette->data, 16);
        
        for (h=0 ; h<16 ; ++h)
            VDP_setPaletteColor(16+h, 0);
    }
    
    int minZ;
    int maxZ;
        
    inline void drawPoly(int a, int b)
    {
        int da = fix16ToInt(smeSCENE.VertexesWS[a].z);
        int db = fix16ToInt(smeSCENE.VertexesWS[b].z);
        int ay = smeSCENE.VertexesVS[a].y;
        int by = smeSCENE.VertexesVS[b].y;
        int y;
        int m = SCENE_Barrel_Depth-1;
        if (by-ay!=0)
        for (y=ay ; y<=by ; ++y)
        {
            int d = da+((db-da)*(y-ay))/(by-ay);
            d = m-((d-minZ)*m)/(maxZ-minZ);
            if (d>m) d = m;
            if (d<0) d = 0;
            
            if (y>=0 && y<screenHeight && d>SCENE_Barrel_DepthBuffer[y])
            {
                SCENE_Barrel_DepthBuffer[y] = d;
                SCENE_Barrel_FrameBuffers[0][y] = SCENE_Barrel_Scanlines[d*SCENE_Barrel_Height+((y-ay)*(SCENE_Barrel_Height-1))/(by-ay)]-y;
            }
        }
    }
    
    void updateTable()
    {
        ++ttt;
        
        smeMESH_SetTranslation(SCENE_Barrel_Mesh, FIX16(0.0f), FIX16(0.0f), FIX16(140.0f));
        smeMESH_SetRotation(SCENE_Barrel_Mesh, FIX16(ttt/4.0f+0.25f), FIX16(0.0f), FIX16(0.0f));
        SCENE_Barrel_Mesh->Transform.rebuildMat = 1;
        M3D_transform(&SCENE_Barrel_Mesh->Transform, SCENE_Barrel_Mesh->Vertexes, smeSCENE.VertexesWS, SCENE_Barrel_Mesh->VertexCount);
        M3D_project_s16(smeSCENE.VertexesWS, smeSCENE.VertexesVS, SCENE_Barrel_Mesh->VertexCount);
        
        int k;
        
        minZ = 9999;
        maxZ = -9999;
        for (k=0 ; k<SCENE_Barrel_Mesh->VertexCount ; ++k)
        {
            if (fix16ToInt(smeSCENE.VertexesWS[k].z)<minZ) minZ = fix16ToInt(smeSCENE.VertexesWS[k].z);
            if (fix16ToInt(smeSCENE.VertexesWS[k].z)>maxZ) maxZ = fix16ToInt(smeSCENE.VertexesWS[k].z);
        }
        if (maxZ==minZ)
            return;
        
        maxZ = (minZ+maxZ)/2+1;
        
        for (k=0 ; k<screenHeight ; ++k)
        {
            SCENE_Barrel_DepthBuffer[k] = 0;
            SCENE_Barrel_FrameBuffers[0][k] = -5-k;
        }
        
        int n;
        for (n=0 ; n<SCENE_Barrel_Mesh->VertexCount ; ++n)
        {
            if ((n+1)%4==0) drawPoly(n, n-3);
            else drawPoly(n, n+1);
        }
        
        memcpy(SCENE_Barrel_FrameBuffers[1], SCENE_Barrel_FrameBuffers[0], screenHeight*sizeof(int));
    }
    
    vu16 *pw = (u16*)GFX_DATA_PORT;
    vu32 *pl = (u32*)GFX_CTRL_PORT;    
    vu32 vscroll_address = GFX_WRITE_VSRAM_ADDR(0);
    int* vscroll_data = SCENE_Barrel_FrameBuffers[1];
    
    void vblank()
    {
        vscroll_data = SCENE_Barrel_FrameBuffers[1];
    } 

    void hblank()
    {
        *pl = vscroll_address;
        *pw = *vscroll_data++;
    }
    
    smeSCREEN_SetScrollMode(smeSCREEN_HSCROLL_Line, smeSCREEN_VSCROLL_Plane);
    
    initTable();
    
    int xs=0, ys=0;
    
    u16 w = SCENE_Barrel_BitmapWidth;
    u16 h = SCENE_Barrel_BitmapHeight;
    VDP_loadBMPTileData((u32*)barrel_bump.image, 1, w/8, h/8, w/8);
    for (ys=0 ; ys<VDP_getPlanHeight()/(h/8); ++ys)
        VDP_fillTileMapRectInc(APLAN, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 0, ys*(h/8), w/8, h/8);
	/*
	w = barrel_bump_back.w;
    h = barrel_bump_back.h;
    VDP_loadBMPTileData((u32*)barrel_bump_back.image, 300, w/8, h/8, w/8);
    for (ys=0 ; ys<VDP_getPlanHeight()/(h/8); ++ys)
        VDP_fillTileMapRectInc(BPLAN, TILE_ATTR_FULL(PAL1, 0, 0, 0, 300), 0, ys*(h/8), w/8, h/8);
	*/

    VDP_clearPlan(BPLAN, 0);
    
    for (ys=0 ; ys<VDP_getPlanHeight(); ++ys)
    for (xs=0 ; xs<12; ++xs)
    {
        VDP_fillTileMapRectInc(BPLAN, TILE_ATTR_FULL(PAL1, 1, 0, 0, 200), xs, ys, 1, 1);
        VDP_fillTileMapRectInc(BPLAN, TILE_ATTR_FULL(PAL1, 1, 0, 0, 200), xs+28, ys, 1, 1);
    }
    
    for (ys=0 ; ys<11; ++ys)
    for (xs=0 ; xs<16; ++xs)
    {
        VDP_fillTileMapRectInc(BPLAN, TILE_ATTR_FULL(PAL1, 1, 0, 0, 200), xs+12, ys, 1, 1);
        VDP_fillTileMapRectInc(BPLAN, TILE_ATTR_FULL(PAL1, 1, 0, 0, 200), xs+12, ys+17, 1, 1);
    }
    
    VDP_setHorizontalScrollLine(PLAN_A, 0, hTable, screenHeight, 0);
    VDP_setHorizontalScrollLine(PLAN_B, 0, hTable2, screenHeight, 0);
    
    SYS_setVIntCallback(vblank);
    SYS_pushHIntCallback(hblank);
    VDP_setHIntCounter(0);
    VDP_setHInterrupt(1); 
   
    curY = 0;
    
    do
    {
        updateTable();
    }
    while(1);
    
    SYS_popHIntCallback();
    SYS_setVIntCallback(0);
    SYS_setHIntCallback(0);
    
    
}
