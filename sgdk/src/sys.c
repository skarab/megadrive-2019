#include "config.h"
#include "types.h"

#include "sys.h"

#include "memory.h"
#include "vdp.h"
#include "vdp_pal.h"
#include "psg.h"
#include "ym2612.h"
#include "joy.h"
#include "z80_ctrl.h"
#include "maths.h"
#include "bmp.h"
#include "timer.h"
#include "string.h"
#include "vdp.h"
#include "vdp_bg.h"
#include "vdp_pal.h"
#include "tile_cache.h"

#include "tools.h"

#define IN_VINT         1
#define IN_HINT         2
#define IN_EXTINT       4


// we don't want to share them
extern u16 randbase;
extern TCBloc *uploads;

// extern library callback function (we don't want to share them)
extern u16 BMP_doHBlankProcess();
extern u16 BMP_doVBlankProcess();
extern u16 TC_doVBlankProcess();
extern u16 SPR_doVBlankProcess();

// main function
extern int main(u16 hard);

static void internal_reset();

// interrrupt callback
_voidCallback *busErrorCB;
_voidCallback *addressErrorCB;
_voidCallback *illegalInstCB;
_voidCallback *zeroDivideCB;
_voidCallback *chkInstCB;
_voidCallback *trapvInstCB;
_voidCallback *privilegeViolationCB;
_voidCallback *traceCB;
_voidCallback *line1x1xCB;
_voidCallback *errorExceptionCB;
_voidCallback *intCB;
_voidCallback *internalVIntCB;
_voidCallback *internalHIntCB;
_voidCallback *internalExtIntCB;

// user V-Int, H-Int and Ext-Int callback
static _voidCallback *VIntCB;
static _voidCallback *HIntCB;
static _voidCallback *ExtIntCB;

u32 registerState[8+8];
u32 pcState;
u32 addrState;
u16 ext1State;
u16 ext2State;
u16 srState;


u32 VIntProcess;
u32 HIntProcess;
u32 ExtIntProcess;
u16 intTrace;

static u16 intLevelSave;
static s16 disableIntStack;


static void addValueU8(char *dst, char *str, u8 value)
{
    char v[16];

    strcat(dst, str);
    intToHex(value, v, 2);
    strcat(dst, v);
}

static void addValueU16(char *dst, char *str, u16 value)
{
    char v[16];

    strcat(dst, str);
    intToHex(value, v, 4);
    strcat(dst, v);
}

static void addValueU32(char *dst, char *str, u32 value)
{
    char v[16];

    strcat(dst, str);
    intToHex(value, v, 8);
    strcat(dst, v);
}

// bus error default callback
void _buserror_callback()
{
    while(1);
}

// address error default callback
void _addresserror_callback()
{
    while(1);
}

// illegal instruction exception default callback
void _illegalinst_callback()
{
    while(1);
}

// division by zero exception default callback
void _zerodivide_callback()
{
    while(1);
}

// CHK instruction default callback
void _chkinst_callback()
{
    while(1);
}

// TRAPV instruction default callback
void _trapvinst_callback()
{
    while(1);
}

// privilege violation exception default callback
void _privilegeviolation_callback()
{
    while(1);
}

// trace default callback
void _trace_callback()
{
}

// line 1x1x exception default callback
void _line1x1x_callback()
{
}

// error exception default callback
void _errorexception_callback()
{
    while(1);
}

// level interrupt default callback
void _int_callback()
{
}


// V-Int Callback
void _vint_callback()
{
    intTrace |= IN_VINT;

    vtimer++;

    // tile cache processing
    if (VIntProcess & PROCESS_TILECACHE_TASK)
    {
        if (!TC_doVBlankProcess()) VIntProcess &= ~PROCESS_TILECACHE_TASK;
    }
    // tile cache processing
    if (VIntProcess & PROCESS_SPRITEENGINE_TASK)
    {
        if (!SPR_doVBlankProcess()) VIntProcess &= ~PROCESS_SPRITEENGINE_TASK;
    }
    // bitmap processing
    if (VIntProcess & PROCESS_BITMAP_TASK)
    {
        if (!BMP_doVBlankProcess()) VIntProcess &= ~PROCESS_BITMAP_TASK;
    }
    // palette fading processing
    if (VIntProcess & PROCESS_PALETTE_FADING)
    {
        if (!VDP_doStepFading(FALSE)) VIntProcess &= ~PROCESS_PALETTE_FADING;
    }

    // ...

    // then call user's callback
    if (VIntCB) VIntCB();

    // joy state refresh (better to do it after user's callback as it can eat some time)
    JOY_update();

    intTrace &= ~IN_VINT;
}

// H-Int Callback
void _hint_callback()
{
    intTrace |= IN_HINT;

    // bitmap processing
    if (HIntProcess & PROCESS_BITMAP_TASK)
    {
        if (!BMP_doHBlankProcess()) HIntProcess &= ~PROCESS_BITMAP_TASK;
    }

    // ...

    // then call user's callback
    if (HIntCB) HIntCB();

    intTrace &= ~IN_HINT;
}

// Ext-Int Callback
void _extint_callback()
{
    intTrace |= IN_EXTINT;

    // processing
//    if (ExtIntProcess & ...)
//    {
//      ...
//    }

    // then call user's callback
    if (ExtIntCB) ExtIntCB();

    intTrace &= ~IN_EXTINT;
}


void _start_entry()
{
    // initiate random number generator
    randbase = 0xD94B ^ GET_HVCOUNTER;
    vtimer = 0;

    // default interrupt callback
    busErrorCB = _buserror_callback;
    addressErrorCB = _addresserror_callback;
    illegalInstCB = _illegalinst_callback;
    zeroDivideCB = _zerodivide_callback;
    chkInstCB = _chkinst_callback;
    trapvInstCB = _trapvinst_callback;
    privilegeViolationCB = _privilegeviolation_callback;
    traceCB = _trace_callback;
    line1x1xCB = _line1x1x_callback;
    errorExceptionCB = _errorexception_callback;
    intCB = _int_callback;
    internalVIntCB = _vint_callback;
    internalHIntCB = _hint_callback;
    internalExtIntCB = _extint_callback;

    internal_reset();

    // let's the fun go on !
    main(1);
}

void _reset_entry()
{
    internal_reset();

    main(0);
}


static void internal_reset()
{
    SYS_popHIntCallback();
    VIntCB = NULL;
    HIntCB = NULL;
    VIntProcess = 0;
    HIntProcess = 0;
    ExtIntProcess = 0;
    intTrace = 0;
    intLevelSave = 0;
    disableIntStack = 0;

    // reset variables which own engine initialization state
    uploads = NULL;

    // init part
    MEM_init();
    VDP_init();
    PSG_init();
    JOY_init();
    // reseting z80 also reset the ym2612
    Z80_init();

    // enable interrupts
    SYS_setInterruptMaskLevel(3);
}

void SYS_disableInts()
{
    // in interrupt --> return
    if (intTrace != 0)
    {
        if (LIB_DEBUG)
            KDebug_Alert("SYS_disableInts() fails: call during interrupt");

        return;
    }

    // disable interrupts
    if (disableIntStack++ == 0)
        intLevelSave = SYS_getAndSetInterruptMaskLevel(7);
    else if (LIB_DEBUG)
        KDebug_Alert("SYS_disableInts() info: inner call");
}

void SYS_enableInts()
{
    // in interrupt --> return
    if (intTrace != 0)
    {
        if (LIB_DEBUG)
            KDebug_Alert("SYS_enableInts() fails: call during interrupt");

        return;
    }

    // reenable interrupts
    if (--disableIntStack == 0)
        SYS_setInterruptMaskLevel(intLevelSave);
    else if (LIB_DEBUG)
    {
        if (disableIntStack < 0)
            KDebug_Alert("SYS_enableInts() fails: already enabled");
        else
            KDebug_Alert("SYS_enableInts() info: inner call");
    }
}

void SYS_setVIntCallback(_voidCallback *CB)
{
    VIntCB = CB;
}

void SYS_setHIntCallback(_voidCallback *CB)
{
    HIntCB = CB;
}

void SYS_pushHIntCallback(_voidCallback *CB)
{
    internalHIntCB = CB;
}

void SYS_popHIntCallback()
{
    internalHIntCB = _hint_callback;
}

void SYS_setExtIntCallback(_voidCallback *CB)
{
    ExtIntCB = CB;
}

u16 SYS_isInVIntCallback()
{
    return intTrace & IN_VINT;
}

u16 SYS_isInHIntCallback()
{
    return intTrace & IN_HINT;
}

u16 SYS_isInExtIntCallback()
{
    return intTrace & IN_EXTINT;
}

u16 SYS_isInInterrupt()
{
    return intTrace;
}


void SYS_die(char *err)
{
    while(1);
}
