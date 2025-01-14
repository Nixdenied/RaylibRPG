// map.c
#include "map.h"
#include <stdlib.h>

Map map;

void InitMap(Map *map, int width, int height)
{
    map->width = width;
    map->height = height;

    // Allocate memory for the 2D tile array
    map->tiles = malloc(height * sizeof(int *));
    for (int y = 0; y < height; y++) {
        map->tiles[y] = malloc(width * sizeof(int));
        for (int x = 0; x < width; x++) {
            // Initialize each tile with a basic terrain type
            if (y < height / 4) {
                map->tiles[y][x] = WATER;  // Water region at the top
            } else if (y > 3 * height / 4) {
                map->tiles[y][x] = MOUNTAIN;  // Mountain region at the bottom
            } else {
                map->tiles[y][x] = TERRAIN;  // Default terrain
            }
        }
    }

    map->unitCount = 0; // Initialize without units; add units as needed
    map->units = NULL;
}

void FreeMap(Map *map)
{
    for (int y = 0; y < map->height; y++) {
        free(map->tiles[y]);
    }
    free(map->tiles);
    if (map->units) {
        free(map->units);
    }
}
