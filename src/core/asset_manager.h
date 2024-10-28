#pragma once

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "tilemap.h"

#define TRANSPARENCY_THRESHOLD 0.99f
#define MAX_TRANSPARENT_PIXELS 0.9f
#define CHUNK_SIZE 64

#define MAX_ANIMATIONS 1000
#define MAX_SPRITES 1000
#define MAX_TILEMAPS 1000
#define ASSET_PATH "../assets/" // Adjust to your asset directory

#define HASH_TABLE_SIZE 1024 // Adjust as needed

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

typedef struct AssetNode
{
    char name[64]; // Asset name (key)
    union
    {
        Sprite sprite; // Store either a sprite or animation
        Animation animation;
        Tilemap tilemap;
    };
    struct AssetNode *next; // Linked list to handle hash collisions
} AssetNode;

typedef struct AssetManager
{
    AssetNode *hashTable[HASH_TABLE_SIZE]; // Hashmap to store assets
    int spriteCount;
    int animationCount;
    int tilemapCount;
    Sprite sprites[MAX_SPRITES];
    Animation animations[MAX_ANIMATIONS];
    Tilemap tilemap[MAX_TILEMAPS];
} AssetManager;

extern AssetManager manager;

// Function prototypes
void InitAssetManager(AssetManager *manager);
void LoadAssetsFromDirectory(AssetManager *manager, const char *directory);
void LoadSprite(AssetManager *manager, const char *filePath);
void LoadAnimation(AssetManager *manager, const char *filePath);
void UpdateAnimations(AssetManager *manager, float deltaTime);
void UnloadAssets(AssetManager *manager);
bool IsFrameBlank(Image texture, Rectangle frame);
void ParseAnimationInfoFromFilename(const char *filePath, char *name, int *rows, int *framesPerRow, int *frameWidth, int *frameHeight);
void AddAssetToHashTable(AssetManager *manager, const char *name, Sprite sprite);
unsigned long HashString(const char *str);
void AddAnimationToHashTable(AssetManager *manager, const char *name, Animation animation);
Sprite GetSprite(AssetManager *manager, const char *name);
Animation GetAnimation(AssetManager *manager, const char *name);
void PrintAllAnimationNames(AssetManager *manager);