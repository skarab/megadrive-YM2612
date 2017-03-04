#include "genesis.h"
#include "sme/sme_input.h"
#include "gfx.h"
#include "ym2612_controller.h"

int LinesCount = 0;
int LineIndex = 0;
int OldLineIndex = -1;
int ElementIndex = 0;
int Move = 0;
int MoveSingle = 0;

void DrawLine(const char* line, int x, int y)
{
    if (LineIndex==LinesCount)
        VDP_setTextPalette(2);

    VDP_drawText(line, x, y);

    if (LineIndex==LinesCount)
        VDP_setTextPalette(0);
}

int k=0;
u8 Initialized = 0;

void YM2612_Display()
{
    ++k;
    if (k>10) k = 0;

    LinesCount = 0;

    char line[256];
    sprintf(line, "%03u %03u %u %u   %02u", YM2612_Chip.Volume, YM2612_Chip.BPM, YM2612_Chip.LFO, YM2612_Chip.LFOFrequency, YM2612_Chip.Position+1);
    DrawLine(line, 12, 1);

    if (LinesCount==LineIndex)
    {
        if (ElementIndex>3) ElementIndex = 0;
        if (ElementIndex<0) ElementIndex = 3;

        if (ElementIndex==0)
        {
            sprintf(line, "%03u", YM2612_Chip.Volume);
            VDP_setTextPalette(3);
            VDP_drawText(line, 12, 1);
            VDP_setTextPalette(0);

            YM2612_Chip.Volume += Move;
        }
        else if (ElementIndex==1)
        {
            sprintf(line, "%03u", YM2612_Chip.BPM);
            VDP_setTextPalette(3);
            VDP_drawText(line, 16, 1);
            VDP_setTextPalette(0);

            YM2612_Chip.BPM += Move;
        }
        else if (ElementIndex==2)
        {
            sprintf(line, "%u", YM2612_Chip.LFO);
            VDP_setTextPalette(3);
            VDP_drawText(line, 20, 1);
            VDP_setTextPalette(0);

            YM2612_Chip.LFO += MoveSingle;
        }
        else if (ElementIndex==3)
        {
            sprintf(line, "%u", YM2612_Chip.LFOFrequency);
            VDP_setTextPalette(3);
            VDP_drawText(line, 22, 1);
            VDP_setTextPalette(0);

            YM2612_Chip.LFOFrequency += MoveSingle;
        }
    }

    ++LinesCount;

    u8 channel_id;
    for (channel_id=0 ; channel_id<6 ; ++channel_id)
    {
        Channel* channel = &YM2612_Chip.Channels[channel_id];
        int y = 3+channel_id*4;

        if (!Initialized || LinesCount==OldLineIndex || LinesCount==LineIndex)
        {
            sprintf(line, "%03u %u%u %05u %u %u %u %u %u %s",
                channel->Volume, channel->OutputLeft, channel->OutputRight,
                channel->Frequency, channel->Algorithm, channel->AMS, channel->FMS,
                channel->Feedback, channel->IsPlaying, channel->Pattern);

            DrawLine(line, 1, y);

            if (LinesCount==LineIndex)
            {
                if (ElementIndex>24) ElementIndex = 0;
                if (ElementIndex<0) ElementIndex = 24;

                if (ElementIndex==0)
                {
                    sprintf(line, "%03u", channel->Volume);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 1, y);
                    VDP_setTextPalette(0);

                    channel->Volume += Move;
                }
                else if (ElementIndex==1)
                {
                    sprintf(line, "%u", channel->OutputLeft);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 5, y);
                    VDP_setTextPalette(0);

                    channel->OutputLeft += MoveSingle;
                }
                else if (ElementIndex==2)
                {
                    sprintf(line, "%u", channel->OutputRight);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 6, y);
                    VDP_setTextPalette(0);

                    channel->OutputRight += MoveSingle;
                }
                else if (ElementIndex==3)
                {
                    sprintf(line, "%05u", channel->Frequency);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 8, y);
                    VDP_setTextPalette(0);

                    channel->Frequency += Move;
                }
                else if (ElementIndex==4)
                {
                    sprintf(line, "%u", channel->Algorithm);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 14, y);
                    VDP_setTextPalette(0);

                    channel->Algorithm += MoveSingle;
                }
                else if (ElementIndex==5)
                {
                    sprintf(line, "%u", channel->AMS);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 16, y);
                    VDP_setTextPalette(0);

                    channel->AMS += MoveSingle;
                }
                else if (ElementIndex==6)
                {
                    sprintf(line, "%u", channel->FMS);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 18, y);
                    VDP_setTextPalette(0);

                    channel->FMS += MoveSingle;
                }
                else if (ElementIndex==7)
                {
                    sprintf(line, "%u", channel->Feedback);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 20, y);
                    VDP_setTextPalette(0);

                    channel->Feedback += MoveSingle;
                }
                else if (ElementIndex==8)
                {
                    sprintf(line, "%u", channel->IsPlaying);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 22, y);
                    VDP_setTextPalette(0);

                    channel->IsPlaying += MoveSingle;
                }
                else if (ElementIndex>8)
                {
                    char txt[17];
                    memcpy(txt, channel->Pattern, 17);

                    if (k>5)
                        txt[ElementIndex-9] = '.';

                    sprintf(line, "%s", txt);
                    VDP_setTextPalette(3);
                    VDP_drawText(line, 24, y);
                    VDP_setTextPalette(0);

                    if (MoveSingle>0) channel->Pattern[ElementIndex-9] = 'A';
                    else if (MoveSingle<0) channel->Pattern[ElementIndex-9] = ' ';
                }
            }
        }

        ++LinesCount;

        u8 op_id;
        for (op_id=0 ; op_id<4 ; ++op_id)
        {
            Operator* op = &channel->Operators[op_id];

            if (!Initialized || LinesCount==OldLineIndex || LinesCount==LineIndex)
            {
                u8 oy = y+1+op_id/2;
                u8 ox = 21*(op_id%2);

                sprintf(line, "%03u%02u%02u%02u%02u%02u %u %02u%u",
                    op->Volume, op->AttackRate, op->FirstDecayRate, op->SecondaryDecayRate,
                    op->SecondaryAmplitude, op->ReleaseRate, op->LFO, op->Mult, op->Detune);
                DrawLine(line, ox, oy);

                if (LinesCount==LineIndex)
                {
                    if (ElementIndex>8) ElementIndex = 0;
                    if (ElementIndex<0) ElementIndex = 8;

                    if (ElementIndex==0)
                    {
                        sprintf(line, "%03u", op->Volume);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox, oy);
                        VDP_setTextPalette(0);

                        op->Volume += Move;
                    }
                    else if (ElementIndex==1)
                    {
                        sprintf(line, "%02u", op->AttackRate);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+3, oy);
                        VDP_setTextPalette(0);

                        op->AttackRate += Move;
                    }
                    else if (ElementIndex==2)
                    {
                        sprintf(line, "%02u", op->FirstDecayRate);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+5, oy);
                        VDP_setTextPalette(0);

                        op->FirstDecayRate += Move;
                    }
                    else if (ElementIndex==3)
                    {
                        sprintf(line, "%02u", op->SecondaryDecayRate);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+7, oy);
                        VDP_setTextPalette(0);

                        op->SecondaryDecayRate += Move;
                    }
                    else if (ElementIndex==4)
                    {
                        sprintf(line, "%02u", op->SecondaryAmplitude);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+9, oy);
                        VDP_setTextPalette(0);

                        op->SecondaryAmplitude += Move;
                    }
                    else if (ElementIndex==5)
                    {
                        sprintf(line, "%02u", op->ReleaseRate);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+11, oy);
                        VDP_setTextPalette(0);

                        op->ReleaseRate += Move;
                    }
                    else if (ElementIndex==6)
                    {
                        sprintf(line, "%u", op->LFO);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+14, oy);
                        VDP_setTextPalette(0);

                        op->LFO += MoveSingle;
                    }
                    else if (ElementIndex==7)
                    {
                        sprintf(line, "%02u", op->Mult);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+16, oy);
                        VDP_setTextPalette(0);

                        op->Mult += MoveSingle;
                    }
                    else if (ElementIndex==8)
                    {
                        sprintf(line, "%u", op->Detune);
                        VDP_setTextPalette(3);
                        VDP_drawText(line, ox+18, oy);
                        VDP_setTextPalette(0);

                        op->Detune += MoveSingle;
                    }
                }
            }

            ++LinesCount;
        }
    }

    Initialized = 1;

    OldLineIndex = -1;

    if (smeINPUT_IsJustPressed(smeINPUT_UP))
    {
        OldLineIndex = LineIndex;
        LineIndex = LineIndex>0?LineIndex-1:LinesCount-1;
    }

    if (smeINPUT_IsJustPressed(smeINPUT_DOWN))
    {
        OldLineIndex = LineIndex;
        LineIndex = LineIndex<LinesCount-1?LineIndex+1:0;
    }

    if (smeINPUT_IsJustPressed(smeINPUT_LEFT)) --ElementIndex;
    if (smeINPUT_IsJustPressed(smeINPUT_RIGHT)) ++ElementIndex;

    Move = 0;
    if (smeINPUT_IsPressed(smeINPUT_A)) ++Move;
    if (smeINPUT_IsPressed(smeINPUT_B)) --Move;

    MoveSingle = 0;
    if (smeINPUT_IsJustPressed(smeINPUT_A)) ++MoveSingle;
    if (smeINPUT_IsJustPressed(smeINPUT_B)) --MoveSingle;
}

int main()
{
    VDP_init();
    VDP_setHInterrupt(0);
    VDP_setHilightShadow(0);
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    smeINPUTS_Initialize();
    YM2612_Initialize();

    VDP_setPaletteColors(16, menu_image.palette->data, 32);
    VDP_drawImageEx(BPLAN, &menu_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX), 0, 0, FALSE, TRUE);

    while(!smeINPUT_IsJustPressed(smeINPUT_START))
    {
        smeINPUTS_Update();
        YM2612_Update();

        VDP_waitVSync();
        VDP_waitVSync();
    }

    VDP_setPaletteColors(16, chip_image.palette->data, 32);
    VDP_drawImageEx(BPLAN, &chip_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX), 0, 0, FALSE, TRUE);

    VDP_setPaletteColor(15, 6|(10<<4)|(6<<8));
    VDP_setPaletteColor(47, 6|(6<<4)|(12<<8));
    VDP_setPaletteColor(63, 15|(0<<4)|(0<<8));

    while(1)
    {
        smeINPUTS_Update();
        YM2612_Update();

        VDP_waitVSync();
        //VDP_showFPS(1);
        YM2612_Display();
    }

    return 0;
}


