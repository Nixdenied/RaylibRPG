#include "raylib.h"
#include "raymath.h"
#include "string.h"
#include "rlgl.h"
#include "debug_scene.h"
#include <stdio.h> // For file operations
#include <stdlib.h>
#include <time.h>
#include "asset_manager.h"

int currentAssetIndex = 0;     // Tracks the currently displayed asset
bool displayingSprites = true; // Flag to determine if displaying sprites or animations
AssetManager assetManager;

void InitDebugScene()
{
    InitAssetManager(&assetManager);
    // Load all assets from the directory
    LoadAssetsFromDirectory(&assetManager, ASSET_PATH);
    // Load assets/shaders/set shader values, for the scene here, place objects
}

void UpdateDebugScene(float deltaTime)
{
    UpdateAnimations(&assetManager, deltaTime);

    // Input handling
    if (IsKeyPressed(KEY_RIGHT))
    {
        if (displayingSprites)
        {
            currentAssetIndex++;
            if (currentAssetIndex >= assetManager.spriteCount)
            {
                currentAssetIndex = 0; // Loop back to first sprite
            }
        }
        else
        {
            currentAssetIndex++;
            if (currentAssetIndex >= assetManager.animationCount)
            {
                currentAssetIndex = 0; // Loop back to first animation
            }
        }
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        if (displayingSprites)
        {
            currentAssetIndex--;
            if (currentAssetIndex < 0)
            {
                currentAssetIndex = assetManager.spriteCount - 1; // Loop to last sprite
            }
        }
        else
        {
            currentAssetIndex--;
            if (currentAssetIndex < 0)
            {
                currentAssetIndex = assetManager.animationCount - 1; // Loop to last animation
            }
        }
    }
    if (IsKeyPressed(KEY_TAB))
    {
        displayingSprites = !displayingSprites; // Toggle between displaying sprites and animations
        currentAssetIndex = 0;                  // Reset index when toggling
    }
}

void RenderDebugScene()
{

    ClearBackground(RAYWHITE);

    // Draw the current asset
    if (displayingSprites && currentAssetIndex < assetManager.spriteCount)
    {
        Sprite *sprite = &assetManager.sprites[currentAssetIndex];
        DrawTexture(sprite->texture, 100, 100, RAYWHITE); // Draw sprite

        // Draw the name above the sprite if the flag is set
        if (sprite->drawName)
        {
            DrawText(sprite->name, 100, 80, 20, BLACK); // Draw the name above the sprite
        }
    }
    else if (!displayingSprites && currentAssetIndex < assetManager.animationCount)
    {
        Animation *anim = &assetManager.animations[currentAssetIndex];
        DrawTextureRec(anim->texture, anim->frames[anim->currentFrame], (Vector2){100, 100}, RAYWHITE); // Draw current animation frame

        // Draw the name above the animation if the flag is set
        if (anim->drawName)
        {
            DrawText(anim->name, 100, 80, 20, BLACK); // Draw the name above the animation
        }
    }

    // BeginMode2D(camera2D);
    // Put draw code in here
    // EndMode2D();
}