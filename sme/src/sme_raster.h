#ifndef _SME_RASTER_H_
#define _SME_RASTER_H_

#include "genesis.h"

inline void smeRASTER_Dot(Vect2D_s16* p, u8 color);
inline void smeRASTER_PolygonDot(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color);

inline void smeRASTER_Wire(Vect2D_s16* p1, Vect2D_s16* p2, u8 color);
inline void smeRASTER_PolygonWire(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color);

inline void smeRASTER_HorizontalLine(int x1, int x2, int y, u8 color);
inline void smeRASTER_PolygonFlat(Vect2D_s16* v1, Vect2D_s16* v2, Vect2D_s16* v3, u8 color);

inline void smeRASTER_HorizontalLineGouraud(long x1, long c1, long x2, long c2, long y);
inline void smeRASTER_PolygonGouraud(Vect2D_s16* v1, u8 c1, Vect2D_s16* v2, u8 c2, Vect2D_s16* v3, u8 c3);

inline void smeRASTER_HorizontalLineTexMap(long x1, long tx1, long ty1, long x2, long tx2, long ty2, long y, const u8* texmap);
inline void smeRASTER_PolygonTexMap(Vect2D_s16* v1, Vect2D_s16* uv1, Vect2D_s16* v2, Vect2D_s16* uv2, Vect2D_s16* v3, Vect2D_s16* uv3, const u8* texmap);

#endif
