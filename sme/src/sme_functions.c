#include "sme_functions.h"

inline void sme_ExchangeInt(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

inline void sme_ExchangeLong(long* a, long* b)
{
    long t = *a;
    *a = *b;
    *b = t;
}

inline void sme_ExchangeU8(u8* a, u8* b)
{
    u8 t = *a;
    *a = *b;
    *b = t;
}
