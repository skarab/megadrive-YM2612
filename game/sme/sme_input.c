#include "sme_input.h"

u16 smeINPUT_Value;
u8 smeINPUT_States[smeINPUT_COUNT*2];

void smeINPUTS_Initialize()
{
    memset(smeINPUT_States, 0, smeINPUT_COUNT*2);
}

void smeINPUT_CheckStatus(u8 button, u8 id)
{
    smeINPUT_States[button*2+1] = smeINPUT_States[button*2];
    smeINPUT_States[button*2] = smeINPUT_Value&id;
}

void smeINPUTS_Update()
{
    smeINPUT_Value = JOY_readJoypad(0);
    smeINPUT_CheckStatus(smeINPUT_UP, BUTTON_UP);
    smeINPUT_CheckStatus(smeINPUT_DOWN, BUTTON_DOWN);
    smeINPUT_CheckStatus(smeINPUT_LEFT, BUTTON_LEFT);
    smeINPUT_CheckStatus(smeINPUT_RIGHT, BUTTON_RIGHT);
    smeINPUT_CheckStatus(smeINPUT_A, BUTTON_A);
    smeINPUT_CheckStatus(smeINPUT_B, BUTTON_B);
    smeINPUT_CheckStatus(smeINPUT_C, BUTTON_C);
    smeINPUT_CheckStatus(smeINPUT_START, BUTTON_START);
}

u8 smeINPUT_IsPressed(u8 button)
{
    return smeINPUT_States[button*2];
}

u8 smeINPUT_IsJustPressed(u8 button)
{
    return smeINPUT_States[button*2] && !smeINPUT_States[button*2+1];
}

u8 smeINPUT_IsJustReleased(u8 button)
{
    return !smeINPUT_States[button*2] && smeINPUT_States[button*2+1];
}

