#include "sme_raster.h"
#include "sme_framebuffer.h"
#include "sme_functions.h"
#include "sme_scene.h"

inline void smeRASTER_Dot(Vect2D_s16* p, u8 color)
{
    if (p->x>=0 && p->x<smeFRAMEBUFFER_Width
        && p->y>=0 && p->y<smeFRAMEBUFFER_Height)
    {
        smeFRAMEBUFFER[smeFRAMEBUFFER_Offsets[p->y]+p->x] = color;
    }
}

inline void smeRASTER_PolygonDot(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color)
{
    smeRASTER_Dot(v1, color);
    smeRASTER_Dot(v2, color);
    smeRASTER_Dot(v3, color);
}

inline void smeRASTER_Wire(Vect2D_s16* p1, Vect2D_s16* p2, u8 color)
{
    char sgndistx1, sgndisty1, sgndistx2, sgndisty2;
    short counter2, distx, disty, absdistx, absdisty;
    unsigned short counter;

    int x1 = p1->x;
    int y1 = p1->y;
    
    distx = p2->x-x1;
    disty = p2->y-y1;
                                  
    if (distx<0) sgndistx1 = -1;
    else if(distx>0) sgndistx1 = 1;
    else sgndistx1 = 0;

    if (disty<0) sgndisty1 = -1;
    else if (disty>0) sgndisty1 = 1;
    else sgndisty1 = 0;

    absdistx = distx;
    if (absdistx<0) absdistx *= -1;

    absdisty = disty;
    if (absdisty<0) absdisty *= -1;

    if (absdistx<=absdisty)
	{
        sgndistx2 = 0;
        sgndisty2 = sgndisty1;
        counter2 = absdistx;
        absdistx = absdisty;
        absdisty = counter2;
	}
    else                   
	{
        sgndistx2 = sgndistx1;
        sgndisty2 = 0;
	}

    counter2 = absdistx >> 1;

    for (counter=0; counter!=absdistx; ++counter)
	{
        if(x1>=0 && x1<smeFRAMEBUFFER_Width
            && y1>=0 && y1<smeFRAMEBUFFER_Height)
        {
            smeFRAMEBUFFER[smeFRAMEBUFFER_Offsets[y1]+x1] = color;
        }
        
        counter2 += absdisty;

        if (counter2>=absdistx)
		{
            counter2 -= absdistx;
            x1 += sgndistx1;
            y1 += sgndisty1;
		}
        else
		{
            x1 += sgndistx2;
            y1 += sgndisty2;
		}
	}
}

inline void smeRASTER_PolygonWire(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color)
{
    smeRASTER_Wire(v1, v2, color);
    smeRASTER_Wire(v2, v3, color);
    smeRASTER_Wire(v3, v1, color);
}

inline void smeRASTER_HorizontalLine(int x1, int x2, int y, u8 color)
{
    if (x1==x2) return;
    if (x1>x2) sme_ExchangeInt(&x1, &x2);
    if (x1>=smeFRAMEBUFFER_Width || x2<0) return;
    if (x1<0) x1 = 0;
    if (x2>=smeFRAMEBUFFER_Width) x2 = smeFRAMEBUFFER_Width-1;
    
    const int start = x1+smeFRAMEBUFFER_Offsets[y];
    const int end = x2+smeFRAMEBUFFER_Offsets[y];
    memset(smeFRAMEBUFFER+start, color, end-start);
}

inline void smeRASTER_PolygonFlat(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color)
{
    if ((v3->x-v1->x)*(v2->y-v1->y) < (v3->y-v1->y)*(v2->x-v1->x))
        return;
    
    color = color|(color<<4);
    Vect2D_s16* p1 = v1;
    Vect2D_s16* p2 = v2;
    Vect2D_s16* p3 = v3;
    Vect2D_s16* t;
    
    if (p2->y>p3->y) { p2 = v3; p3 = v2; }
    if (p1->y>p3->y) { p1 = p3; p3 = v1; }
    if (p1->y>p2->y) { t = p1; p1 = p2; p2 = t; }
    if (p3->y==p1->y) return;
    
    int ix1 = (p3->x-p1->x)*65536/(p3->y-p1->y);

    if (p2->y!=p1->y && p1->y<smeFRAMEBUFFER_Height && p2->y>=0)
    {
        int ix2 = (p2->x-p1->x)*65536/(p2->y-p1->y);
        int dx1 = p1->x*65536;
        int dx2 = dx1;
        int start = p1->y;
        int end = p2->y;
        if (start<0) { int d=-start; dx1+=ix1*d; dx2+=ix2*d; start=0; }
        if (end>=smeFRAMEBUFFER_Height) end=smeFRAMEBUFFER_Height-1;
        int ly;
        for (ly=start ; ly<end ; ++ly)
        {
            smeRASTER_HorizontalLine(dx1/65536, dx2/65536, ly, color);
            dx1 += ix1;
            dx2 += ix2;
        }
    }

    if (p3->y!=p2->y && p2->y<smeFRAMEBUFFER_Height && p3->y>=0)
    {
        int ix2 = (p3->x-p2->x)*65536/(p3->y-p2->y);
        int dx1 = p1->x*65536+((p2->y-p1->y)*ix1);
        int dx2 = p2->x*65536;
        int start = p2->y;
        int end = p3->y;
        if (start<0) { int d=-start; dx1+=ix1*d; dx2+=ix2*d; start=0; }
        if (end>=smeFRAMEBUFFER_Height) end=smeFRAMEBUFFER_Height-1;
        int ly;
        for (ly=start ; ly<end ; ++ly)
        {
            smeRASTER_HorizontalLine(dx1/65536, dx2/65536, ly, color);
            dx1 += ix1;
            dx2 += ix2;
        }
    }
}

inline void smeRASTER_HorizontalLineGouraud(long x1, long c1, long x2, long c2, long y)
{
    if (x1==x2) return;
    if (x1>x2) { sme_ExchangeLong(&x1, &x2); sme_ExchangeLong(&c1, &c2); }
    if (x1>=smeFRAMEBUFFER_Width || x2<0) return;
    long ic = (c2-c1)/(x2-x1); 
    if (x1<0) { c1 += -x1*ic; x1 = 0; }
    if (x2>=smeFRAMEBUFFER_Width) x2 = smeFRAMEBUFFER_Width-1;
    u8* p = smeFRAMEBUFFER+smeFRAMEBUFFER_Offsets[y]+x1;
    u8* end = smeFRAMEBUFFER+smeFRAMEBUFFER_Offsets[y]+x2+1;
    while (p!=end)
    {
        *p = c1/65536|((c1/65536)<<4);
        ++p;
        c1 += ic;
    }
}

inline void smeRASTER_PolygonGouraud(Vect2D_s16* v1, u8 c1, Vect2D_s16* v2, u8 c2, Vect2D_s16* v3, u8 c3)
{
    if ((v3->x-v1->x)*(v2->y-v1->y) < (v3->y-v1->y)*(v2->x-v1->x))
        return;
    
    Vect2D_s16* p1 = v1;
    Vect2D_s16* p2 = v2;
    Vect2D_s16* p3 = v3;
    Vect2D_s16* t;
    
    if (p2->y>p3->y) { p2 = v3; p3 = v2; sme_ExchangeU8(&c2, &c3); }
    if (p1->y>p3->y) { p1 = p3; p3 = v1; sme_ExchangeU8(&c1, &c3); }
    if (p1->y>p2->y) { t = p1; p1 = p2; p2 = t; sme_ExchangeU8(&c1, &c2); }
    if (p3->y==p1->y) return;

    long ix1 = (p3->x-p1->x)*65536/(p3->y-p1->y);
    long ic1 = (c3-c1)*65536/(p3->y-p1->y);
    
    if (p2->y!=p1->y)
    {
        long ix2 = (p2->x-p1->x)*65536/(p2->y-p1->y);
        long ic2 = (c2-c1)*65536/(p2->y-p1->y);
        long dx1 = p1->x*65536;
        long dxc1 = c1*65536;
        long dx2 = dx1;
        long dxc2 = dxc1;
        long ly;
        for (ly=p1->y ; ly<p2->y ; ++ly)
        {
            if (ly>=0 && ly<smeFRAMEBUFFER_Height)
                smeRASTER_HorizontalLineGouraud(dx1/65536, dxc1, dx2/65536, dxc2, ly);
            
            dx1 += ix1;
            dx2 += ix2;
            dxc1 += ic1;
            dxc2 += ic2;
        }
    }

    if (p3->y!=p2->y)
    {
        long ix2 = (p3->x-p2->x)*65536/(p3->y-p2->y);
        long ic2 = (c3-c2)*65536/(p3->y-p2->y);
        long dx1 = p1->x*65536+((p2->y-p1->y)*ix1);
        long dxc1 = c1*65536+((p2->y-p1->y)*ic1);
        long dx2 = p2->x*65536;
        long dxc2 = c2*65536;
        long ly;
        for (ly=p2->y ; ly<p3->y ; ++ly)
        {
            if (ly>=0 && ly<smeFRAMEBUFFER_Height)
                smeRASTER_HorizontalLineGouraud(dx1/65536, dxc1, dx2/65536, dxc2, ly);
            
            dx1 += ix1;
            dx2 += ix2;
            dxc1 += ic1;
            dxc2 += ic2;
        }
    }
}

inline void smeRASTER_HorizontalLineTexMap(long x1, long tx1, long ty1, long x2, long tx2, long ty2, long y, const u8* texmap)
{
    if (x1==x2) return;
    if (x1>x2) { sme_ExchangeLong(&x1, &x2); sme_ExchangeLong(&tx1, &tx2); sme_ExchangeLong(&ty1, &ty2); }
    if (x1>=smeFRAMEBUFFER_Width || x2<0) return;
    long ix = (tx2-tx1)/(x2-x1); 
    long iy = (ty2-ty1)/(x2-x1); 
    if (x1<0) { tx1 += -x1*ix; ty1 += -x1*iy; x1 = 0; }
    if (x2>=smeFRAMEBUFFER_Width) x2 = smeFRAMEBUFFER_Width-1;
    u8* p = smeFRAMEBUFFER+smeFRAMEBUFFER_Offsets[y]+x1;
    u8* end = smeFRAMEBUFFER+smeFRAMEBUFFER_Offsets[y]+x2+1;
    while (p!=end)
    {
        long t = ((tx1>>17)+((ty1>>11)&0xFF00))%(32*64);
        *p = texmap[t];
        ++p;
        tx1 += ix;
        ty1 += iy;
    }
}

inline void smeRASTER_PolygonTexMap(Vect2D_s16* v1, Vect2D_s16* uv1, Vect2D_s16* v2, Vect2D_s16* uv2, Vect2D_s16* v3, Vect2D_s16* uv3, const u8* texmap)
{
    if ((v3->x-v1->x)*(v2->y-v1->y) < (v3->y-v1->y)*(v2->x-v1->x))
        return;
            
    Vect2D_s16* p1[2]; p1[0] = v1; p1[1] = uv1;
    Vect2D_s16* p2[2]; p2[0] = v2; p2[1] = uv2;
    Vect2D_s16* p3[2]; p3[0] = v3; p3[1] = uv3;
    Vect2D_s16* t;
    
    if (p2[0]->y>p3[0]->y) { p2[0] = v3; p3[0] = v2; p2[1] = uv3; p3[1] = uv2; }
    if (p1[0]->y>p3[0]->y) { p1[0] = p3[0]; p3[0] = v1; p1[1] = p3[1]; p3[1] = uv1; }
    if (p1[0]->y>p2[0]->y) { t = p1[0]; p1[0] = p2[0]; p2[0] = t; t = p1[1]; p1[1] = p2[1]; p2[1] = t; }
    if (p3[0]->y==p1[0]->y) return;

    long ix1 = (p3[0]->x-p1[0]->x)*65536/(p3[0]->y-p1[0]->y);
    long iuv1[2]; iuv1[0] = (p3[1]->x-p1[1]->x)*65536/(p3[0]->y-p1[0]->y); iuv1[1] = (p3[1]->y-p1[1]->y)*65536/(p3[0]->y-p1[0]->y);
    
    if (p2[0]->y!=p1[0]->y)
    {
        long ix2 = (p2[0]->x-p1[0]->x)*65536/(p2[0]->y-p1[0]->y);
        long iuv2[2]; iuv2[0] = (p2[1]->x-p1[1]->x)*65536/(p2[0]->y-p1[0]->y); iuv2[1] = (p2[1]->y-p1[1]->y)*65536/(p2[0]->y-p1[0]->y);
        long dx1 = p1[0]->x*65536;
        long dxuv1[2]; dxuv1[0] = p1[1]->x*65536; dxuv1[1] = p1[1]->y*65536;
        long dx2 = dx1;
        long dxuv2[2]; dxuv2[0] = dxuv1[0]; dxuv2[1] = dxuv1[1];
        long ly;
        for (ly=p1[0]->y ; ly<p2[0]->y ; ++ly)
        {
            if (ly>=0 && ly<smeFRAMEBUFFER_Height)
                smeRASTER_HorizontalLineTexMap(dx1/65536, dxuv1[0], dxuv1[1], dx2/65536, dxuv2[0], dxuv2[1], ly, texmap);
            dx1 += ix1; dx2 += ix2;
            dxuv1[0] += iuv1[0]; dxuv1[1] += iuv1[1];
            dxuv2[0] += iuv2[0]; dxuv2[1] += iuv2[1];
        }
    }

    if (p3[0]->y!=p2[0]->y)
    {
        long ix2 = (p3[0]->x-p2[0]->x)*65536/(p3[0]->y-p2[0]->y);
        long iuv2[2]; iuv2[0] = (p3[1]->x-p2[1]->x)*65536/(p3[0]->y-p2[0]->y); iuv2[1] = (p3[1]->y-p2[1]->y)*65536/(p3[0]->y-p2[0]->y);
        long dx1 = p1[0]->x*65536+((p2[0]->y-p1[0]->y)*ix1);
        long dxuv1[1]; dxuv1[0] = p1[1]->x*65536+((p2[0]->y-p1[0]->y)*iuv1[0]); dxuv1[1] = p1[1]->y*65536+((p2[0]->y-p1[0]->y)*iuv1[1]);
        long dx2 = p2[0]->x*65536;
        long dxuv2[2]; dxuv2[0] = p2[1]->x*65536; dxuv2[1] = p2[1]->y*65536;        
        long ly;
        for (ly=p2[0]->y ; ly<p3[0]->y ; ++ly)
        {
            if (ly>=0 && ly<smeFRAMEBUFFER_Height)
                smeRASTER_HorizontalLineTexMap(dx1/65536, dxuv1[0], dxuv1[1], dx2/65536, dxuv2[0], dxuv2[1], ly, texmap);
            dx1 += ix1; dx2 += ix2;
            dxuv1[0] += iuv1[0]; dxuv1[1] += iuv1[1];
            dxuv2[0] += iuv2[0]; dxuv2[1] += iuv2[1];
        }
    }
}
