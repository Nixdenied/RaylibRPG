#include "raylib.h"
#include "asset_manager.h"
#include <stdio.h>

void PrintAllSpriteNames(AssetManager *manager) {
    printf("Sprite Names:\n");
    for (int i = 0; i < manager->spriteCount; i++) {
        printf("Sprite %d: %s\n", i + 1, manager->sprites[i].name);
    }
}


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
