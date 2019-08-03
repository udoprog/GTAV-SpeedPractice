#pragma once

#include "types.h"

void ScriptKeyboardHandler(DWORD, WORD, BYTE, BOOL, BOOL, BOOL, BOOL);

bool IsKeyDown(DWORD key);

bool IsKeyJustUp(DWORD key, bool exclusive);

void ResetKeyState(DWORD key);