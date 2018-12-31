#ifndef _SME_SCREEN_H_
#define _SME_SCREEN_H_

#include "genesis.h"

#define smeSCREEN_HSCROLL_Plane  0
#define smeSCREEN_HSCROLL_Row    2
#define smeSCREEN_HSCROLL_Line   3
#define smeSCREEN_VSCROLL_Plane  0
#define smeSCREEN_VSCROLL_Column 4

void smeSCREEN_SetScrollMode(u8 horizontal, u8 vertical);

#endif
