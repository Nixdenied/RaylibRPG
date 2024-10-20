#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#define TRANSPARENCY_THRESHOLD 0.99f
#define MAX_TRANSPARENT_PIXELS 0.9f

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

// Function prototypes
void InitAssetManager(AssetManager *manager);
void LoadAssetsFromDirectory(AssetManager *manager, const char *directory);
void LoadSprite(AssetManager *manager, const char *filePath);
void LoadAnimation(AssetManager *manager, const char *filePath);
void UpdateAnimations(AssetManager *manager, float deltaTime);
void UnloadAssets(AssetManager *manager);
bool IsFrameBlank(Texture2D texture, Rectangle frame);
void ParseAnimationInfoFromFilename(const char *filePath, char *name, int *rows, int *framesPerRow, int *frameWidth, int *frameHeight);

#endif // ASSET_MANAGER_H
