#include "raylib.h"
#include "raymath.h"
#include "string.h"
#include "rlgl.h"
#include "debug_scene.h"
#include <stdio.h> // For file operations
#include <stdlib.h>
#include <time.h>
#include "asset_manager.h"
#include "tile_placement_data.h"
#include "npc.h"
#include "raylib_utils.h"

Vector2 squarePosition = {600, 400}; // Initial position of the square
const float squareSpeed = 200.0f;    // Speed of the square
Rectangle squareBounds;              // Bounds of the square

// Assuming you have a maximum number of NPCs
#define MAX_NPCS 10

// Add these global or scene-specific variables
NPC npcs[MAX_NPCS];
int npcCount = 0; // Current number of NPCs

bool CheckCollisionWithTiles(Rectangle square)
{
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            // Check collision for each collidable tile in the stack
            for (int i = 0; i < stack->count; i++)
            {
                if (stack->isCollidable[i]) // Only check collidable tiles
                {
                    // Calculate the tile rectangle
                    Rectangle tileRect = {
                        x * tileSize,
                        y * tileSize,
                        tileSize,
                        tileSize};

                    if (CheckCollisionRecs(square, tileRect))
                    {
                        return true; // Collision detected
                    }
                }
            }
        }
    }
    return false; // No collision
}

void InitTestMapScene()
{
    InitAssetManager(&manager);
    LoadAssetsFromDirectory(&manager, ASSET_PATH);
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Initialize map size and screen size
    InitTileData(256, 256, screenWidth, screenHeight); // Initialize with default map size
    LoadFirstMapInDirectory("../maps");                // Load a map from your specified directory
    PrintAllAnimationNames(&manager);

    if (npcCount < MAX_NPCS)
    {
        InitNPC(&npcs[npcCount], (Vector2){300, 300}, 100.0f, "idle_animation");
        npcs[npcCount].drawName = true; // Optional: Display NPC's name
        npcCount++;
    }
}

void UpdateTestMapScene(float deltaTime)
{
    squareBounds = (Rectangle){squarePosition.x, squarePosition.y, 50, 50}; // Update square bounds

    UpdateAnimations(&manager, deltaTime);

    Vector2 newPosition = squarePosition; // Store new position for collision checking
    if (IsKeyDown(KEY_W))
        newPosition.y -= squareSpeed * deltaTime; // Move up
    if (IsKeyDown(KEY_S))
        newPosition.y += squareSpeed * deltaTime; // Move down
    if (IsKeyDown(KEY_A))
        newPosition.x -= squareSpeed * deltaTime; // Move left
    if (IsKeyDown(KEY_D))
        newPosition.x += squareSpeed * deltaTime; // Move right

    // Check collision before updating position
    squareBounds = (Rectangle){newPosition.x, newPosition.y, 50, 50};
    if (!CheckCollisionWithTiles(squareBounds))
    {
        squarePosition = newPosition; // Only update position if no collision
    }

    // Update NPCs
    for (int i = 0; i < npcCount; i++)
    {
        UpdateNPC(&npcs[i], deltaTime);

        if (npcs[i].state == NPC_WALKING)
        {
            // Move NPC in the current walking direction
            npcs[i].position.x += npcs[i].speed * deltaTime;

            // Check boundaries to change state to idle
            if (npcs[i].position.x >= 500) // Right boundary
            {
                SetNPCState(&npcs[i], NPC_IDLE);
                npcs[i].speed = -fabs(npcs[i].speed); // Set speed to negative for moving left
            }
            else if (npcs[i].position.x <= 100) // Left boundary
            {
                SetNPCState(&npcs[i], NPC_IDLE);
                npcs[i].speed = fabs(npcs[i].speed); // Set speed to positive for moving right
            }
        }
        else if (npcs[i].state == NPC_IDLE)
        {
            // After idling, switch back to walking
            SetNPCState(&npcs[i], NPC_WALKING);
        }
    }
}
void RenderTestMapScene()
{
    ClearBackground(GetColorFromHex("#47aaa9"));

    // Draw placed tiles and sprites on the grid
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            // Render the tiles from bottom to top first
            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000; // Determine which tilemap the tile belongs to
                int tileIndex = stack->tiles[i] % 1000;    // Get the tile index

                // Only draw the tile from the tilemap if it exists
                if (tileIndex < manager.tilemap[tilemapIndex].totalTiles)
                {
                    // Draw tile from tilemap
                    DrawTexture(manager.tilemap[tilemapIndex].tiles[tileIndex], x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

    // Draw placed tiles and sprites on the grid
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            // Render the tiles from bottom to top first
            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000; // Determine which tilemap the tile belongs to
                int tileIndex = stack->tiles[i] % 1000;    // Get the tile index

                // Only draw the tile from the tilemap if it exists
                if (tileIndex < manager.tilemap[tilemapIndex].totalTiles)
                {
                    // Draw tile from tilemap
                    DrawTexture(manager.tilemap[tilemapIndex].tiles[tileIndex], x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

    // Now draw all the sprites on top of the tiles
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                // Check if it is a sprite
                if (tileIndex >= manager.tilemap[tilemapIndex].totalTiles)
                {
                    // Draw sprite on top of the tile
                    DrawTexture(manager.sprites[tileIndex - manager.tilemap[tilemapIndex].totalTiles].texture, x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

        // Step 3: Draw all animations on top of both tiles and sprites
    for (int y = 0; y < mapTilesY; y++)
    {
        for (int x = 0; x < mapTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                // Draw only animations
                int animationIndex = tileIndex - manager.tilemap[tilemapIndex].totalTiles - manager.spriteCount;
                if (animationIndex >= 0 && animationIndex < manager.animationCount)
                {
                    Animation *anim = &manager.animations[animationIndex];
                    Rectangle currentFrame = anim->frames[anim->currentFrame];
                    DrawTextureRec(anim->texture, currentFrame, (Vector2){x * tileSize, y * tileSize}, WHITE);
                }
            }
        }
    }

    // Draw NPCs on top of the tiles
    for (int i = 0; i < npcCount; i++)
    {
        DrawNPC(&npcs[i]);
    }

    // Draw the controllable square
    DrawRectangle(squarePosition.x, squarePosition.y, 50, 50, BLUE); // A blue square of 50x50
}