#pragma once

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define TRANSPARENCY_THRESHOLD 0.99f
#define MAX_TRANSPARENT_PIXELS 0.9f
#define CHUNK_SIZE 64

#define MAX_ANIMATIONS 1000
#define MAX_SPRITES 1000
#define ASSET_PATH "../assets/" // Adjust to your asset directory

typedef struct Animation
{
    Texture2D texture;
    int frameCount;
    int currentFrame;
    float frameTime;
    float elapsedTime;
    Rectangle *frames; // Array of rectangles for each frame
    int frameWidth;    // Frame width parsed from filename
    int frameHeight;   // Frame height parsed from filename
    int rows;          // Rows parsed from filename
    int framesPerRow;  // Frames per row parsed from filename
    char name[64];     // Sprite name extracted from the filename
    bool drawName;     // Flag to determine if the name should be drawn
} Animation;

typedef struct Sprite
{
    Texture2D texture;
    char name[64]; // Sprite name extracted from the filename
    bool drawName; // Flag to determine if the name should be drawn
} Sprite;

typedef struct AssetManager
{
    Animation animations[MAX_ANIMATIONS];
    Sprite sprites[MAX_SPRITES];
    int spriteCount;
    int animationCount;
} AssetManager;

// Define a Tile struct that can hold tile data (adjust fields as needed)
typedef struct Tile {
    int type;        // Tile type or ID (you can change this to anything else)
    bool isWalkable; // Whether the tile can be walked on or not
    Texture2D texture; // The texture of the tile
} Tile;

// Define a Chunk struct, which contains a 64x64 grid of tiles
typedef struct Chunk {
    Tile tiles[CHUNK_SIZE][CHUNK_SIZE]; // 64x64 grid of tiles
    Vector2 position; // Position of the chunk in the world
} Chunk;

// Define a TileMap struct to manage multiple chunks
typedef struct TileMap {
    Chunk **chunks; // 2D array of pointers to chunks (dynamically allocated)
    int chunkRows;  // Number of chunk rows
    int chunkCols;  // Number of chunk columns
} TileMap;

extern AssetManager manager;

// Function prototypes
void InitAssetManager(AssetManager *manager);
void LoadAssetsFromDirectory(AssetManager *manager, const char *directory);
void LoadSprite(AssetManager *manager, const char *filePath);
void LoadAnimation(AssetManager *manager, const char *filePath);
void UpdateAnimations(AssetManager *manager, float deltaTime);
void UnloadAssets(AssetManager *manager);
bool IsFrameBlank(Texture2D texture, Rectangle frame);
void ParseAnimationInfoFromFilename(const char *filePath, char *name, int *rows, int *framesPerRow, int *frameWidth, int *frameHeight);