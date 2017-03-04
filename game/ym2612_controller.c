#include "ym2612_controller.h"

#define CLAMP(x, min, max) { if (x<min) x = min; else if (x>max) x = max; }

YM2612 YM2612_Chip;

void Operator_Initialize(Operator* op, u8 id, u8 channel)
{
    op->ID = id;
    op->Channel = channel;
    op->Volume = 118;
    op->AttackRate = 16;
    op->FirstDecayRate = 8;
    op->SecondaryDecayRate = 6;
    op->SecondaryAmplitude = 15;
    op->ReleaseRate = 4;
    op->LFO = 1;
    op->Mult = 0;
    op->Detune = 0;
}

void Operator_Apply(Operator* op, u8 channel_volume, u8 play_mode)
{
    u8 part = op->Channel>2?1:0;
    u8 chan = op->Channel<3?op->Channel:op->Channel-3;
    u8 id = op->ID;

    CLAMP(op->Detune, 0, 7);
    CLAMP(op->Mult, 0, 15);
    YM2612_writeReg(part, 0x30+chan+id*4, (op->Detune<<4)|op->Mult);

    CLAMP(op->Volume, 0, 127);
    u8 volume = (u8)(127-(int)op->Volume*(int)channel_volume/127);
    CLAMP(volume, 0, 127);
    YM2612_writeReg(part, 0x40+chan+id*4, volume);

    CLAMP(op->AttackRate, 0, 31);
    YM2612_writeReg(part, 0x50+chan+id*4, op->AttackRate);

    CLAMP(op->LFO, 0, 1);
    CLAMP(op->FirstDecayRate, 0, 31);
    u8 val = op->FirstDecayRate;
    if (play_mode==2) val = 0;
    YM2612_writeReg(part, 0x60+chan+id*4, (op->LFO<<7)|val);

    CLAMP(op->SecondaryDecayRate, 0, 31);
    val = op->SecondaryDecayRate;
    if (play_mode==2) val = 0;
    YM2612_writeReg(part, 0x70+chan+id*4, (op->LFO<<7)|val);

    CLAMP(op->SecondaryAmplitude, 0, 15);
    CLAMP(op->ReleaseRate, 0, 15);
    val = op->ReleaseRate;
    if (play_mode==2) val = 0;
    YM2612_writeReg(part, 0x80+chan+id*4, (op->SecondaryAmplitude<<4)|val);
}

const char Patterns[][16] = {
    "A  A    A  A    ",
    " A    A  A   A A",
    "   A    A       ",
    " A              ",
    "A A  A          ",
    "A A A A A A A A "};

void Channel_Initialize(Channel* channel, u8 id)
{
    channel->ID = id;

    channel->Volume = 127;
    channel->Algorithm = id+1;
    channel->Feedback = 0;
    channel->OutputLeft = 1;
    channel->OutputRight = 1;
    channel->AMS = 0;
    channel->FMS = 0;

    u8 op_id;
    for (op_id=0 ; op_id<4 ; ++op_id)
        Operator_Initialize(&channel->Operators[op_id], op_id, id);

    channel->Frequency = 8809;
    memcpy(channel->Pattern, Patterns[id], 16);
    channel->Pattern[16] = 0;

    channel->IsPlaying = 1;
    channel->KeyPressed = 0;
}

void Channel_Apply(Channel* channel, u8 volume)
{
    u8 part = channel->ID>2?1:0;
    u8 chan = channel->ID<3?channel->ID:channel->ID-3;

    CLAMP(channel->Volume, 0, 127);
    u8 vol = (u8)((int)channel->Volume*(int)volume/127);

    CLAMP(channel->IsPlaying, 0, 2);

    u8 op_id;
    for (op_id=0 ; op_id<4 ; ++op_id)
        Operator_Apply(&channel->Operators[op_id], vol, channel->IsPlaying);

    CLAMP(channel->Frequency, (u16)0, (u16)80000);
    YM2612_writeReg(part, 0xA4+chan, channel->Frequency>>8);
    YM2612_writeReg(part, 0xA0+chan, channel->Frequency&255);

    CLAMP(channel->Feedback, 0, 7);
    CLAMP(channel->Algorithm, 0, 7);
    YM2612_writeReg(part, 0xB0+chan, (channel->Feedback<<3)|channel->Algorithm);

    CLAMP(channel->OutputLeft, 0, 1);
    CLAMP(channel->OutputRight, 0, 1);
    CLAMP(channel->AMS, 0, 3);
    CLAMP(channel->FMS, 0, 7);
    YM2612_writeReg(part, 0xB4+chan, (channel->OutputLeft<<7)|(channel->OutputRight<<6)|(channel->FMS<<3)|channel->AMS);
}

void YM2612_Initialize()
{
    YM2612_Chip.Volume = 127;
    YM2612_Chip.BPM = 120;
    YM2612_Chip.LFO = 1;
    YM2612_Chip.LFOFrequency = 4;

    u8 channel_id;
    for (channel_id=0 ; channel_id<6 ; ++channel_id)
        Channel_Initialize(&YM2612_Chip.Channels[channel_id], channel_id);

    YM2612_Chip.Position = 0;
    YM2612_Chip.Time = 0;
}

const int Channels[] = {0, 1, 2, 4, 5, 6};

void YM2612_Update()
{
    CLAMP(YM2612_Chip.BPM, 40, 500);
    CLAMP(YM2612_Chip.LFO, 0, 1);
    CLAMP(YM2612_Chip.LFOFrequency, 0, 7);

    YM2612_writeReg(0, 0x22, (YM2612_Chip.LFO<<3)|YM2612_Chip.LFOFrequency);
    YM2612_writeReg(0, 0x27, 0);
    YM2612_writeReg(0, 0x2B, 0);

    CLAMP(YM2612_Chip.Volume, 0, 127);

    u8 channel_id;
    for (channel_id=0 ; channel_id<6 ; ++channel_id)
        Channel_Apply(&YM2612_Chip.Channels[channel_id], YM2612_Chip.Volume);

    // Sequencer

    //if (IS_PALSYSTEM) YM2612_Chip.Time += FIX16(1.0f/50.0f);
    //else YM2612_Chip.Time += FIX16(1.0f/60.0f);
    YM2612_Chip.Time += FIX16(1.0f/30.0f);

    fix16 step = fix16Div(FIX16(60.0), FIX16(YM2612_Chip.BPM));

    if (YM2612_Chip.Time>=step)
    {
        YM2612_Chip.Time -= step;
        YM2612_Chip.Position = (YM2612_Chip.Position+1)%16;

        u8 channel_id;
        for (channel_id=0 ; channel_id<6 ; ++channel_id)
        {
            Channel* chan = &YM2612_Chip.Channels[channel_id];
            if (chan->IsPlaying==0)
            {
                if (chan->KeyPressed==1)
                {
                    YM2612_writeReg(0, 0x28, Channels[channel_id]);
                    chan->KeyPressed = 0;
                }
            }
            else if (chan->IsPlaying==1)
            {
                YM2612_writeReg(0, 0x28, Channels[channel_id]);
                chan->KeyPressed = 0;

                if (chan->Pattern[YM2612_Chip.Position]!=' ')
                {
                    YM2612_writeReg(0, 0x28, (15<<4)|Channels[channel_id]);
                    chan->KeyPressed = 1;
                }
            }
            else if (chan->KeyPressed==0)
            {
                YM2612_writeReg(0, 0x28, (15<<4)|Channels[channel_id]);
                chan->KeyPressed = 1;
            }
        }
    }
}
