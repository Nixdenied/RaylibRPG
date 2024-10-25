// tile_placement_data.c

#include "tile_placement_data.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

const int tileSize = 64;
int screenTilesX;
int screenTilesY;
int selectedTileIndex = 0;
int selectedSpriteIndex = 0;
int selectedTilemapIndex = 0;

Rectangle saveButton = {10, 10, 100, 30};
Rectangle loadButton = {120, 10, 100, 30};

TileStack **placedTiles = NULL;

void InitTileData(int screenWidth, int screenHeight)
{
    screenTilesX = screenWidth / tileSize;
    screenTilesY = screenHeight / tileSize;

    if (placedTiles != NULL)
        FreeTileData();

    placedTiles = (TileStack **)malloc(screenTilesY * sizeof(TileStack *));
    for (int i = 0; i < screenTilesY; i++)
    {
        placedTiles[i] = (TileStack *)malloc(screenTilesX * sizeof(TileStack));
        for (int j = 0; j < screenTilesX; j++)
        {
            placedTiles[i][j].tiles = NULL;
            placedTiles[i][j].count = 0;
            placedTiles[i][j].capacity = 0;
            placedTiles[i][j].isCollidable = false;
        }
    }
}

void FreeTileData()
{
    if (placedTiles != NULL)
    {
        for (int i = 0; i < screenTilesY; i++)
        {
            if (placedTiles[i] != NULL)
            {
                for (int j = 0; j < screenTilesX; j++)
                {
                    free(placedTiles[i][j].tiles);
                    placedTiles[i][j].tiles = NULL;

                    // Free the isCollidable array
                    free(placedTiles[i][j].isCollidable);  // Free the collidable states array
                    placedTiles[i][j].isCollidable = NULL; // Reset pointer to NULL
                }
                free(placedTiles[i]);
            }
        }
        free(placedTiles);
        placedTiles = NULL;
    }
}

void SaveTilePlacement(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
        return;

    fwrite(&screenTilesX, sizeof(int), 1, file);
    fwrite(&screenTilesY, sizeof(int), 1, file);

    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            fwrite(&stack->count, sizeof(int), 1, file);
            fwrite(stack->tiles, sizeof(int), stack->count, file);
            fwrite(stack->isCollidable, sizeof(bool), stack->count, file); // Save the collidable states
        }
    }

    fclose(file);
}

void LoadTilePlacement(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return;

    fread(&screenTilesX, sizeof(int), 1, file);
    fread(&screenTilesY, sizeof(int), 1, file);

    FreeTileData();
    placedTiles = (TileStack **)malloc(screenTilesY * sizeof(TileStack *));
    for (int y = 0; y < screenTilesY; y++)
    {
        placedTiles[y] = (TileStack *)malloc(screenTilesX * sizeof(TileStack));
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            fread(&stack->count, sizeof(int), 1, file);

            if (stack->count > 0)
            {
                stack->tiles = (int *)malloc(stack->count * sizeof(int));
                fread(stack->tiles, sizeof(int), stack->count, file);

                stack->isCollidable = (bool *)malloc(stack->count * sizeof(bool)); // Allocate memory for collidable states
                fread(stack->isCollidable, sizeof(bool), stack->count, file);      // Load the collidable states
            }
            else
            {
                stack->tiles = NULL;
                stack->isCollidable = NULL; // Initialize collidable pointer to NULL if no tiles
                stack->capacity = 0;
            }
        }
    }

    fclose(file);
}

void LoadFirstMapInDirectory(const char *directory)
{
    DIR *dir = opendir(directory);
    if (dir == NULL)
    {
        printf("Failed to open directory: %s\n", directory);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG) // Regular file
        {
            char filePath[256];
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, entry->d_name);
            printf("Loading map from: %s\n", filePath);

            FreeTileData(); // Free any previous data
            LoadTilePlacement(filePath);
            break;
        }
    }

    closedir(dir);
}

void PushTileToStack(TileStack *stack, int tileIndex, bool isCollidable)
{
    if (stack->count >= stack->capacity)
    {
        stack->capacity = (stack->capacity == 0) ? 4 : stack->capacity * 2;
        stack->tiles = (int *)realloc(stack->tiles, stack->capacity * sizeof(int));
        stack->isCollidable = realloc(stack->isCollidable, stack->capacity * sizeof(bool));
    }
    stack->tiles[stack->count] = tileIndex;
    stack->isCollidable[stack->count] = isCollidable;
    stack->count++;
}