#ifndef _SME_INPUT_H_
#define _SME_INPUT_H_

#include "genesis.h"

#define smeINPUT_UP       0
#define smeINPUT_DOWN     1
#define smeINPUT_LEFT     2
#define smeINPUT_RIGHT    3
#define smeINPUT_A        4
#define smeINPUT_B        5
#define smeINPUT_C        6
#define smeINPUT_START    7
#define smeINPUT_COUNT    8

void smeINPUTS_Initialize();
void smeINPUTS_Update();

u8 smeINPUT_IsPressed(u8 button);
u8 smeINPUT_IsJustPressed(u8 button);
u8 smeINPUT_IsJustReleased(u8 button);

#endif
