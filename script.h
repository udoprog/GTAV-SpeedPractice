#pragma once

#include "natives.h"
#include "types.h"
#include "enums.h"

#include "main.h"

class Mission;

void ScriptMain();

// Show a subtitle with the given content.
void ShowSubtitle(const char* s);

// Draw a notification with the given content.
void Notify(const char* s);