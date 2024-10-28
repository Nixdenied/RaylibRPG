// tile_placement_data.c

#include "tile_placement_data.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

// Global Variables
const int tileSize = 64; // Size of each tile in pixels
int mapTilesX = 256;     // Total map width in tiles
int mapTilesY = 256;     // Total map height in tiles
int screenTilesX = 0;    // Number of tiles horizontally visible on screen
int screenTilesY = 0;    // Number of tiles vertically visible on screen
int selectedTileIndex = 0;
int selectedSpriteIndex = 0;
int selectedTilemapIndex = 0;
int selectedAnimationIndex = 0;

Rectangle saveButton = {10, 10, 100, 30};
Rectangle loadButton = {120, 10, 100, 30};

TileStack **placedTiles = NULL;

// Track the current allocated size
int allocatedTilesX = 0;
int allocatedTilesY = 0;

void InitTileData(int newMapWidth, int newMapHeight, int screenWidth, int screenHeight)
{
    mapTilesX = newMapWidth;
    mapTilesY = newMapHeight;
    screenTilesX = screenWidth / tileSize;
    screenTilesY = screenHeight / tileSize;

    if (placedTiles != NULL)
        FreeTileData();

    // Initialize allocated size to map size
    allocatedTilesX = mapTilesX;
    allocatedTilesY = mapTilesY;

    placedTiles = (TileStack **)malloc(allocatedTilesY * sizeof(TileStack *));
    if (!placedTiles)
    {
        fprintf(stderr, "Failed to allocate memory for placedTiles.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < allocatedTilesY; i++)
    {
        placedTiles[i] = (TileStack *)malloc(allocatedTilesX * sizeof(TileStack));
        if (!placedTiles[i])
        {
            fprintf(stderr, "Failed to allocate memory for placedTiles[%d].\n", i);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < allocatedTilesX; j++)
        {
            placedTiles[i][j].tiles = NULL;
            placedTiles[i][j].isCollidable = NULL;
            placedTiles[i][j].count = 0;
            placedTiles[i][j].capacity = 0;
        }
    }
}

void FreeTileData()
{
    if (placedTiles != NULL)
    {
        for (int i = 0; i < allocatedTilesY; i++)
        {
            if (placedTiles[i] != NULL)
            {
                for (int j = 0; j < allocatedTilesX; j++)
                {
                    // Free tiles array
                    if (placedTiles[i][j].tiles != NULL)
                    {
                        free(placedTiles[i][j].tiles);
                        placedTiles[i][j].tiles = NULL;
                    }

                    // Free isCollidable array
                    if (placedTiles[i][j].isCollidable != NULL)
                    {
                        free(placedTiles[i][j].isCollidable);
                        placedTiles[i][j].isCollidable = NULL;
                    }

                    // Reset count and capacity
                    placedTiles[i][j].count = 0;
                    placedTiles[i][j].capacity = 0;
                }
                free(placedTiles[i]);
                placedTiles[i] = NULL;
            }
        }
        free(placedTiles);
        placedTiles = NULL;
        allocatedTilesX = 0;
        allocatedTilesY = 0;
    }
}

void SaveTilePlacement(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for saving: %s\n", filename);
        return;
    }

    // Write map dimensions
    fwrite(&mapTilesX, sizeof(int), 1, file);
    fwrite(&mapTilesY, sizeof(int), 1, file);

    for (int y = 0; y < mapTilesY; y++)
    {
        for (int x = 0; x < mapTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            fwrite(&stack->count, sizeof(int), 1, file);

            if (stack->count > 0)
            {
                fwrite(stack->tiles, sizeof(int), stack->count, file);
                fwrite(stack->isCollidable, sizeof(bool), stack->count, file); // Save collidability states
            }
        }
    }

    fclose(file);
    printf("Map saved to %s successfully.\n", filename);
}

void LoadTilePlacement(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for loading: %s\n", filename);
        return;
    }

    // Read map dimensions
    fread(&mapTilesX, sizeof(int), 1, file);
    fread(&mapTilesY, sizeof(int), 1, file);

    // Free old data based on previous allocatedTilesX/Y
    FreeTileData();

    // Now set the new allocated sizes
    allocatedTilesX = mapTilesX;
    allocatedTilesY = mapTilesY;

    placedTiles = (TileStack **)malloc(allocatedTilesY * sizeof(TileStack *));
    if (!placedTiles)
    {
        fprintf(stderr, "Failed to allocate memory for placedTiles during loading.\n");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < allocatedTilesY; y++)
    {
        placedTiles[y] = (TileStack *)malloc(allocatedTilesX * sizeof(TileStack));
        if (!placedTiles[y])
        {
            fprintf(stderr, "Failed to allocate memory for placedTiles[%d] during loading.\n", y);
            exit(EXIT_FAILURE);
        }

        for (int x = 0; x < allocatedTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            fread(&stack->count, sizeof(int), 1, file);

            if (stack->count > 0)
            {
                stack->tiles = (int *)malloc(stack->count * sizeof(int));
                if (!stack->tiles)
                {
                    fprintf(stderr, "Failed to allocate memory for tiles in placedTiles[%d][%d].\n", y, x);
                    exit(EXIT_FAILURE);
                }
                fread(stack->tiles, sizeof(int), stack->count, file);

                stack->isCollidable = (bool *)malloc(stack->count * sizeof(bool));
                if (!stack->isCollidable)
                {
                    fprintf(stderr, "Failed to allocate memory for isCollidable in placedTiles[%d][%d].\n", y, x);
                    exit(EXIT_FAILURE);
                }
                fread(stack->isCollidable, sizeof(bool), stack->count, file);

                stack->capacity = stack->count;
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
    printf("Map loaded from %s successfully.\n", filename);
}

// Comparator function for sorting file names alphabetically
int CompareFileNames(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
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
    struct stat fileStat;
    char **fileNames = NULL;
    size_t fileCount = 0;

    // Collect all file names
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/%s", directory, entry->d_name);
        if (stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode))
        {
            fileNames = realloc(fileNames, sizeof(char *) * (fileCount + 1));
            fileNames[fileCount] = strdup(filePath); // Store full file path
            fileCount++;
        }
    }
    closedir(dir);

    // Sort file names alphabetically
    qsort(fileNames, fileCount, sizeof(char *), CompareFileNames);

    // Load the first sorted map file, if any
    if (fileCount > 0)
    {
        LoadTilePlacement(fileNames[0]);
    }

    // Free file names array
    for (size_t i = 0; i < fileCount; i++)
    {
        free(fileNames[i]);
    }
    free(fileNames);
}


void PushTileToStack(TileStack *stack, int tileIndex, bool isCollidable)
{
    if (stack->count >= stack->capacity)
    {
        int newCapacity = (stack->capacity == 0) ? 4 : stack->capacity * 2;
        int *newTiles = (int *)realloc(stack->tiles, newCapacity * sizeof(int));
        if (!newTiles)
        {
            fprintf(stderr, "Failed to realloc tiles in PushTileToStack.\n");
            exit(EXIT_FAILURE);
        }
        stack->tiles = newTiles;

        bool *newCollidable = (bool *)realloc(stack->isCollidable, newCapacity * sizeof(bool));
        if (!newCollidable)
        {
            fprintf(stderr, "Failed to realloc isCollidable in PushTileToStack.\n");
            exit(EXIT_FAILURE);
        }
        stack->isCollidable = newCollidable;

        stack->capacity = newCapacity;
    }
    stack->tiles[stack->count] = tileIndex;
    stack->isCollidable[stack->count] = isCollidable;
    stack->count++;
}
