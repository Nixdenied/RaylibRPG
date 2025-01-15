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

#include <stdint.h>

uint32_t htonl(uint32_t hostlong) {
    return ((hostlong & 0x000000FF) << 24) |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0xFF000000) >> 24);
}

uint32_t ntohl(uint32_t netlong) {
    // Since the operation is symmetric, it's identical to htonl
    return htonl(netlong);
}

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

void SaveTilePlacement(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file for saving: %s\n", filename);
        return;
    }

    // Write metadata
    int version = 1; // File format version
    version = htonl(version);
    fwrite(&version, sizeof(int), 1, file);

    int dimensions[2] = { htonl(mapTilesX), htonl(mapTilesY) };
    fwrite(dimensions, sizeof(int), 2, file);

    for (int y = 0; y < mapTilesY; y++) {
        for (int x = 0; x < mapTilesX; x++) {
            TileStack *stack = &placedTiles[y][x];
            int count = htonl(stack->count);
            fwrite(&count, sizeof(int), 1, file);

            for (int i = 0; i < stack->count; i++) {
                int tile = htonl(stack->tiles[i]);
                bool collidable = stack->isCollidable[i];
                fwrite(&tile, sizeof(int), 1, file);
                fwrite(&collidable, sizeof(bool), 1, file);
            }
        }
    }

    fclose(file);
    printf("Map saved to %s successfully.\n", filename);
}

void LoadTilePlacement(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file for loading: %s\n", filename);
        return;
    }

    int version;
    fread(&version, sizeof(int), 1, file);
    version = ntohl(version);

    if (version != 1) {
        fprintf(stderr, "Unsupported file version: %d\n", version);
        fclose(file);
        return;
    }

    int dimensions[2];
    fread(dimensions, sizeof(int), 2, file);
    mapTilesX = ntohl(dimensions[0]);
    mapTilesY = ntohl(dimensions[1]);

    FreeTileData();
    InitTileData(mapTilesX, mapTilesY, screenTilesX * tileSize, screenTilesY * tileSize);

    for (int y = 0; y < mapTilesY; y++) {
        for (int x = 0; x < mapTilesX; x++) {
            TileStack *stack = &placedTiles[y][x];

            int count;
            fread(&count, sizeof(int), 1, file);
            count = ntohl(count);

            if (count > 0) {
                stack->tiles = (int *)malloc(count * sizeof(int));
                stack->isCollidable = (bool *)malloc(count * sizeof(bool));
                stack->capacity = count;
                stack->count = count;

                for (int i = 0; i < count; i++) {
                    fread(&stack->tiles[i], sizeof(int), 1, file);
                    stack->tiles[i] = ntohl(stack->tiles[i]);
                    fread(&stack->isCollidable[i], sizeof(bool), 1, file);
                }
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
