#pragma once
#include <stdint.h>

enum SPECIAL_TIMERS
{
    Timer_None = 0,
    Timer_Compiling = -1,
    Timer_RSP_Running = -2,
    Timer_RDP_Running = -3,
};

void ResetTimerList(void);
uint32_t StartTimer(uint32_t Address);
void StopTimer(void);
void GenerateTimerResults(void);
