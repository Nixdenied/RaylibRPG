#pragma once

#include "raylib.h"

Color GetColorFromHex(const char *hex);
unsigned long HashString(const char *str);
void PrintAllSpriteNames(AssetManager *manager);