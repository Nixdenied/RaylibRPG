#pragma once

#include "raylib.h"

typedef struct Tilemap {
    Texture2D *tiles;       // Array of tile textures
    int tileCountX;         // Number of tiles horizontally
    int tileCountY;         // Number of tiles vertically
    int totalTiles;         // Total number of tiles
    int currentTileIndex;   // Currently selected tile index
} Tilemap;

extern Tilemap tilemap;

// Function declarations
Tilemap LoadTilemap(const char *filePath, int tileSize);
void UpdateTilemap(Tilemap *tilemap);
void UnloadTilemap(Tilemap *tilemap);
