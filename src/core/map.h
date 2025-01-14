// map.h
#pragma once
#include "raylib.h"

#define WATER 0
#define MOUNTAIN 1
#define TERRAIN 2  // Default terrain
#define PLAYER_TEAM 0
#define ENEMY_TEAM 1

// Map structure definition
typedef struct Map {
    int width;
    int height;
    int **tiles;  // 2D array of integers for tile types
    int unitCount;
    struct Unit {
        Vector2 position;
        int team; // PLAYER_TEAM or ENEMY_TEAM for factions
    } *units;
} Map;

extern Map map;

// Function to initialize a map with specified dimensions and tile types
void InitMap(Map *map, int width, int height);

// Function to free map resources
void FreeMap(Map *map);
