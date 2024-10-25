#include "raylib.h"
#include <stdio.h>

Color GetColorFromHex(const char *hex)
{
    int red, green, blue;
    sscanf(hex + 1, "%02x%02x%02x", &red, &green, &blue); // Skip '#' and read RGB values
    return (Color){red, green, blue, 255};                // Return color with full opacity
}

unsigned long HashString(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}
