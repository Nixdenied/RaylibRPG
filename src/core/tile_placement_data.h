// tile_placement_data.h

#pragma once

#include <stdbool.h>
#include "raylib.h" // For Rectangle

typedef struct {
    int *tiles;             // Array of tile indices
    bool *isCollidable;     // Array of collidability states
    int count;              // Number of tiles
    int capacity;           // Capacity of the tiles array
} TileStack;

// Function Declarations
void InitTileData(int mapWidth, int mapHeight, int screenWidth, int screenHeight);
void FreeTileData();
void SaveTilePlacement(const char *filename);
void LoadTilePlacement(const char *filename);
void LoadFirstMapInDirectory(const char *directory);
void PushTileToStack(TileStack *stack, int tileIndex, bool isCollidable);

// External Variables
extern const int tileSize;
extern int mapTilesX;
extern int mapTilesY;
extern int screenTilesX;
extern int screenTilesY;
extern int selectedTileIndex;
extern int selectedSpriteIndex;
extern int selectedTilemapIndex;
extern int selectedAnimationIndex;

extern Rectangle saveButton;
extern Rectangle loadButton;

extern TileStack **placedTiles;
