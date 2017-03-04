#include "genesis.h"

typedef struct
{
    u8 ID;
    u8 Channel;
    u8 Detune;
    u8 Mult;
    u8 Volume;
    u8 LFO;
    u8 AttackRate;
    u8 FirstDecayRate;
    u8 SecondaryDecayRate;
    u8 SecondaryAmplitude;
    u8 ReleaseRate;
} Operator;

typedef struct
{
    u8 ID;
    u8 Volume;
    u16 Frequency;
    u8 Algorithm;
    u8 Feedback;
    u8 OutputLeft;
    u8 OutputRight;
    u8 AMS;
    u8 FMS;
    Operator Operators[4];
    u8 IsPlaying;
    u8 KeyPressed;
    char Pattern[17];
} Channel;

typedef struct
{
    u8 Volume;
    u16 BPM;
    u8 LFO;
    u8 LFOFrequency;
    Channel Channels[6];
    u8 Position;
    fix16 Time;
} YM2612;

extern YM2612 YM2612_Chip;

void YM2612_Initialize();
void YM2612_Update();
