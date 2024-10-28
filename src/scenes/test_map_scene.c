#include "raylib.h"
#include "raymath.h"
#include "string.h"
#include "rlgl.h"
#include "debug_scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asset_manager.h"
#include "tile_placement_data.h"
#include "npc.h"
#include "raylib_utils.h"
#include "buildings.h" // Include the building header
#include "custom_cursor.h"

#define MAX_BUILDINGS 5

Vector2 squarePosition = {600, 400}; // Initial position of the square
const float squareSpeed = 200.0f;    // Speed of the square
Rectangle squareBounds;              // Bounds of the square
static bool isSelecting = false;   // Whether a selection box is active
static Vector2 selectionStart;     // Start point of the drag
static Vector2 selectionEnd;       // End point of the drag



// Global variables for NPCs and buildings
NPC npcs[MAX_NPCS];

Building buildings[MAX_BUILDINGS];
int buildingCount = 0; // Current number of buildings

bool CheckCollisionWithTiles(Rectangle square)
{
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            for (int i = 0; i < stack->count; i++)
            {
                if (stack->isCollidable[i])
                {
                    Rectangle tileRect = {x * tileSize, y * tileSize, tileSize, tileSize};
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

void UpdateDragSelection(NPC *npcs, int npcCount)
{
    Vector2 mousePosition = GetMousePosition();

    // Start drag selection on mouse left button press
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        selectionStart = mousePosition;
        isSelecting = true;
    }

    // Update the end position of the drag as the mouse moves
    if (isSelecting)
    {
        selectionEnd = mousePosition;
    }

    // Complete selection on mouse left button release
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && isSelecting)
    {
        Rectangle selectionBox = {
            fmin(selectionStart.x, selectionEnd.x),
            fmin(selectionStart.y, selectionEnd.y),
            fabs(selectionEnd.x - selectionStart.x),
            fabs(selectionEnd.y - selectionStart.y)
        };

        // Check which NPCs are within the selection box
        for (int i = 0; i < npcCount; i++)
        {
            NPC *npc = &npcs[i];
            Rectangle npcRect = {
                npc->position.x - npc->animation.frameWidth / 2,
                npc->position.y - npc->animation.frameHeight / 2,
                npc->animation.frameWidth,
                npc->animation.frameHeight
            };

            if (CheckCollisionRecs(selectionBox, npcRect))
            {
                npc->isSelected = true;
            }
            else
            {
                npc->isSelected = false;
            }
        }

        isSelecting = false; // Reset selection box
    }
}

void InitTestMapScene()
{
    InitAssetManager(&manager);
    LoadAssetsFromDirectory(&manager, ASSET_PATH);
    LoadNewAssets(&manager, "../assets/Tiny Swords (Update 010)/");
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    InitCustomCursor(&manager);

    // Initialize map size and screen size
    InitTileData(256, 256, screenWidth, screenHeight);
    LoadFirstMapInDirectory("../maps");
    PrintAllAnimationNames(&manager);
    PrintAllSpriteNames(&manager);

    // Initialize an NPC if there is room
    if (npcCount < MAX_NPCS)
    {
        InitNPC(&npcs[npcCount], &manager, (Vector2){300, 300}, 100.0f, "WarriorRed_1");
        npcs[npcCount].drawName = true;
        npcCount++;
    }

    // Initialize a building if there is room
    if (buildingCount < MAX_BUILDINGS)
    {
        InitBuilding(&buildings[buildingCount], (Vector2){400, 400}, BUILDING_TYPE_UNIT_PRODUCER, &manager);
        buildingCount++;
    }
}

void UpdateTestMapScene(float deltaTime)
{
    squareBounds = (Rectangle){squarePosition.x, squarePosition.y, 50, 50}; // Update square bounds

    UpdateAnimations(&manager, deltaTime);
    UpdateCustomCursor(npcs, npcCount, buildings, buildingCount);

    Vector2 newPosition = squarePosition;
    if (IsKeyDown(KEY_W))
        newPosition.y -= squareSpeed * deltaTime;
    if (IsKeyDown(KEY_S))
        newPosition.y += squareSpeed * deltaTime;
    if (IsKeyDown(KEY_A))
        newPosition.x -= squareSpeed * deltaTime;
    if (IsKeyDown(KEY_D))
        newPosition.x += squareSpeed * deltaTime;

    // Check collision before updating position
    squareBounds = (Rectangle){newPosition.x, newPosition.y, 100, 100};
    if (!CheckCollisionWithTiles(squareBounds))
    {
        squarePosition = newPosition;
    }

    // Handle mouse input for NPC selection and movement
    Vector2 mousePosition = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    HandleNPCMouseInput(npcs, npcCount, mousePosition, mousePressed);

    // Update NPCs
    for (int i = 0; i < npcCount; i++)
    {
        UpdateNPC(&npcs[i], npcs, deltaTime);
    }

    // Handle building selection
    // Check building clicks after NPCs for exclusive handling
    HandleBuildingClick(buildings, buildingCount, npcs, npcCount, mousePosition, mousePressed);

    // Update Buildings
    for (int i = 0; i < buildingCount; i++)
    {
        UpdateBuilding(&buildings[i], npcs, &npcCount, &manager, deltaTime);
    }

    UpdateDragSelection(npcs, npcCount);
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
            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                if (tileIndex < manager.tilemap[tilemapIndex].totalTiles)
                {
                    DrawTexture(manager.tilemap[tilemapIndex].tiles[tileIndex], x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

    // Draw sprites on top of the tiles
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                if (tileIndex >= manager.tilemap[tilemapIndex].totalTiles)
                {
                    DrawTexture(manager.sprites[tileIndex - manager.tilemap[tilemapIndex].totalTiles].texture, x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

    // Draw animations
    for (int y = 0; y < mapTilesY; y++)
    {
        for (int x = 0; x < mapTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];
            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;
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

    // Draw Buildings and selection UI
    for (int i = 0; i < buildingCount; i++)
    {
        DrawBuilding(&buildings[i]);
        if (buildings[i].isSelected)
        {
            RenderUnitSelectionUI(&buildings[i]);
        }
    }

    // Draw the controllable square
    DrawRectangle(squarePosition.x, squarePosition.y, 50, 50, BLUE);

        // Draw the selection box if in the middle of a drag
    if (isSelecting)
    {
        Rectangle selectionBox = {
            fmin(selectionStart.x, selectionEnd.x),
            fmin(selectionStart.y, selectionEnd.y),
            fabs(selectionEnd.x - selectionStart.x),
            fabs(selectionEnd.y - selectionStart.y)
        };

        DrawRectangleLinesEx(selectionBox, 2, GREEN); // Outline of the selection box
        DrawRectangle(selectionBox.x, selectionBox.y, selectionBox.width, selectionBox.height, Fade(GREEN, 0.3f)); // Semi-transparent fill
    }

    DrawCustomCursor();
}
