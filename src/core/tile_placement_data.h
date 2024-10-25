// tile_placement_data.h
#pragma once

#include "raylib.h"

// Structure definitions
typedef struct TileStack
{
    int *tiles;   
    int count;    
    int capacity;
    bool *isCollidable;
} TileStack;

// Button definitions
extern Rectangle saveButton;
extern Rectangle loadButton;

// External declarations of data structures and variables
extern TileStack **placedTiles;
extern int screenTilesX;
extern int screenTilesY;
extern int selectedTileIndex;
extern int selectedSpriteIndex;
extern int selectedTilemapIndex;
extern const int tileSize;

// Function prototypes
void InitTileData(int screenWidth, int screenHeight);
void FreeTileData();
void SaveTilePlacement(const char *filename);
void LoadTilePlacement(const char *filename);
void LoadFirstMapInDirectory(const char *directory);
void PushTileToStack(TileStack *stack, int tileIndex, bool isCollidable);
