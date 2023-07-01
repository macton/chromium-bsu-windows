#pragma once
#include <stdint.h>
#include "vec2.h"

enum InputPadButton
{
    kInputPadUp              = 0,
    kInputPadDown            = 1,
    kInputPadLeft            = 2,
    kInputPadRight           = 3,
    kInputPadButtonPrimary   = 4,
    kInputPadButtonSecondary = 5,
    kInputPadHome            = 6,
};

extern uint32_t g_InputPadDown;

void Input_Reset();