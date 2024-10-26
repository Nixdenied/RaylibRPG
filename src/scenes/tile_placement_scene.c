// tile_placement_scene.c

#include "tile_placement_scene.h"
#include "tilemap.h"
#include "asset_manager.h"
#include <stdlib.h>
#include <dirent.h>
#include "tile_placement_data.h"
#include "raylib_utils.h"
#include <stdbool.h>
#include "raymath.h"

// External Variables
extern AssetManager manager;
extern TileStack **placedTiles;
extern Rectangle saveButton;
extern Rectangle loadButton;
extern int screenTilesX;
extern int screenTilesY;
extern const int tileSize;
extern int mapTilesX;
extern int mapTilesY;
extern int selectedTilemapIndex;
extern int selectedTileIndex;
extern int selectedSpriteIndex;
extern int selectedAnimationIndex;

// Global Variables
static Vector2 mousePosition;
static Camera2D camera;
static Vector2 prevMousePosition;
bool isTileCollidable = false; // Global collidability state

// Function Declarations
void InitTilePlacementScene();
void UpdateTilePlacementScene(float deltaTime);
void RenderTilePlacementScene();
void UnloadTilePlacementScene();

void InitTilePlacementScene()
{
    InitAssetManager(&manager);
    LoadAssetsFromDirectory(&manager, ASSET_PATH);
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Initialize map size and screen size
    InitTileData(256, 256, screenWidth, screenHeight); // Initialize with default map size

    // Initialize camera settings
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Log tile information
    if (manager.tilemap && manager.tilemap->tiles)
    {
        for (int i = 0; i < manager.tilemapCount; i++)
        {
            int totalTiles = manager.tilemap[i].totalTiles;
            if (totalTiles > 0)
            {
                printf("Loaded %d tiles in tilemap %d.\n", totalTiles, i);
            }
            else
            {
                printf("No tiles loaded in manager.tilemap[%d].tiles.\n", i);
            }
        }
    }
    else
    {
        printf("Tilemap or tiles array is not initialized.\n");
    }
}

void UpdateTilePlacementScene(float deltaTime)
{
    // Update mouse position at the start
    mousePosition = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePosition, camera);
    UpdateAnimations(&manager, deltaTime);

    // Handle camera movement
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        Vector2 delta = Vector2Subtract(mousePosition, prevMousePosition);
        camera.target = Vector2Subtract(camera.target, Vector2Scale(delta, 1.0f / camera.zoom));
    }

    // Update previous mouse position for next frame
    prevMousePosition = mousePosition;

    // Calculate which tile the mouse is over in world space
    int tileX = (int)(worldMousePos.x / tileSize);
    int tileY = (int)(worldMousePos.y / tileSize);

    // Detect if the Load button is clicked (GUI interaction)
    if (CheckCollisionPointRec(mousePosition, loadButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        LoadFirstMapInDirectory("../maps");
        printf("First map loaded from ../maps\n");
    }

    // Detect if the Save button is clicked (GUI interaction)
    if (CheckCollisionPointRec(mousePosition, saveButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        SaveTilePlacement("../maps/map1.dat");
        printf("Map saved to ../maps/map1.dat\n");
    }

    // Check if tile coordinates are valid (World interaction)
    if (tileX >= 0 && tileX < mapTilesX && tileY >= 0 && tileY < mapTilesY)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            int tileIndex = -1;

            if (selectedTileIndex >= 0 && selectedTileIndex < manager.tilemap[selectedTilemapIndex].totalTiles)
            {
                tileIndex = (selectedTilemapIndex * 1000) + selectedTileIndex;
            }
            else if (selectedSpriteIndex >= 0 && selectedSpriteIndex < manager.spriteCount)
            {
                tileIndex = (selectedTilemapIndex * 1000) + manager.tilemap[selectedTilemapIndex].totalTiles + selectedSpriteIndex;
            }
            else if (selectedAnimationIndex >= 0 && selectedAnimationIndex < manager.animationCount)
            {
                tileIndex = (selectedTilemapIndex * 1000) + manager.tilemap[selectedTilemapIndex].totalTiles + manager.spriteCount + selectedAnimationIndex;
            }

            if (tileIndex != -1)
            {
                printf("Placing tile at (%d, %d) with tileIndex: %d\n", tileX, tileY, tileIndex);
                PushTileToStack(&placedTiles[tileY][tileX], tileIndex, isTileCollidable);
            }
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            // Remove the top tile from the stack if it exists
            if (placedTiles[tileY][tileX].count > 0)
            {
                placedTiles[tileY][tileX].count--;
                printf("Removed top tile from (%d, %d)\n", tileX, tileY);
            }
        }
    }

    // Handle collidability toggle and tile selection
    if (IsKeyPressed(KEY_C))
    {
        isTileCollidable = !isTileCollidable;
        printf("Collidability set to: %s\n", isTileCollidable ? "ON" : "OFF");
    }

    // Change selected tile within the current tilemap
    if (IsKeyPressed(KEY_LEFT))
    {
        selectedTileIndex = (selectedTileIndex - 1 + manager.tilemap[selectedTilemapIndex].totalTiles) % manager.tilemap[selectedTilemapIndex].totalTiles;
        selectedSpriteIndex = -1; // Deactivate sprite selection when switching to tiles
        printf("Selected tile index: %d\n", selectedTileIndex);
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        selectedTileIndex = (selectedTileIndex + 1) % manager.tilemap[selectedTilemapIndex].totalTiles;
        selectedSpriteIndex = -1; // Deactivate sprite selection when switching to tiles
        printf("Selected tile index: %d\n", selectedTileIndex);
    }

    // Change selected sprite with UP/DOWN keys
    if (IsKeyPressed(KEY_UP))
    {
        selectedSpriteIndex = (selectedSpriteIndex - 1 + manager.spriteCount) % manager.spriteCount;
        selectedTileIndex = -1; // Deactivate tile selection when switching to sprites
        printf("Selected sprite index: %d\n", selectedSpriteIndex);
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        selectedSpriteIndex = (selectedSpriteIndex + 1) % manager.spriteCount;
        selectedTileIndex = -1; // Deactivate tile selection when switching to sprites
        printf("Selected sprite index: %d\n", selectedSpriteIndex);
    }

    // Switch between tilemaps with PAGE_UP and PAGE_DOWN
    if (IsKeyPressed(KEY_PAGE_UP))
    {
        selectedTilemapIndex = (selectedTilemapIndex - 1 + manager.tilemapCount) % manager.tilemapCount;
        printf("Selected tilemap index: %d\n", selectedTilemapIndex);
    }
    if (IsKeyPressed(KEY_PAGE_DOWN))
    {
        selectedTilemapIndex = (selectedTilemapIndex + 1) % manager.tilemapCount;
        printf("Selected tilemap index: %d\n", selectedTilemapIndex);
    }

    // Select animations with A and S keys
    if (IsKeyPressed(KEY_A))
    {
        selectedAnimationIndex = (selectedAnimationIndex - 1 + manager.animationCount) % manager.animationCount;
        selectedTileIndex = selectedSpriteIndex = -1;
        printf("Selected animation index: %d\n", selectedAnimationIndex);
    }
    if (IsKeyPressed(KEY_S))
    {
        selectedAnimationIndex = (selectedAnimationIndex + 1) % manager.animationCount;
        selectedTileIndex = selectedSpriteIndex = -1;
        printf("Selected animation index: %d\n", selectedAnimationIndex);
    }
}

void RenderTilePlacementScene()
{
    ClearBackground(GetColorFromHex("#47aaa9"));

    // Begin camera mode for world rendering
    BeginMode2D(camera);

    // Step 1: Draw all tiles at the bottom layer
    for (int y = 0; y < mapTilesY; y++)
    {
        for (int x = 0; x < mapTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                // Draw only tiles from tilemaps
                if (tileIndex < manager.tilemap[tilemapIndex].totalTiles)
                {
                    DrawTexture(manager.tilemap[tilemapIndex].tiles[tileIndex], x * tileSize, y * tileSize, WHITE);
                }
            }
        }
    }

    // Step 2: Draw all sprites on top of tiles
    for (int y = 0; y < mapTilesY; y++)
    {
        for (int x = 0; x < mapTilesX; x++)
        {
            TileStack *stack = &placedTiles[y][x];

            for (int i = 0; i < stack->count; i++)
            {
                int tilemapIndex = stack->tiles[i] / 1000;
                int tileIndex = stack->tiles[i] % 1000;

                // Draw only sprites
                if (tileIndex >= manager.tilemap[tilemapIndex].totalTiles &&
                    tileIndex < manager.tilemap[tilemapIndex].totalTiles + manager.spriteCount)
                {
                    int spriteIndex = tileIndex - manager.tilemap[tilemapIndex].totalTiles;
                    DrawTexture(manager.sprites[spriteIndex].texture, x * tileSize, y * tileSize, WHITE);
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

    // Draw the grid lines
    for (int x = 0; x <= mapTilesX; x++)
    {
        DrawLine(x * tileSize, 0, x * tileSize, mapTilesY * tileSize, BLACK);
    }
    for (int y = 0; y <= mapTilesY; y++)
    {
        DrawLine(0, y * tileSize, mapTilesX * tileSize, y * tileSize, BLACK);
    }

    // Retrieve the current mouse position in world space
    Vector2 worldMousePos = GetScreenToWorld2D(GetMousePosition(), camera);

    // Display the currently selected tile or sprite or animation at the mouse position
    if (selectedTileIndex >= 0 && selectedTileIndex < manager.tilemap[selectedTilemapIndex].totalTiles)
    {
        // Display the selected tile from the currently selected tilemap
        DrawTexture(manager.tilemap[selectedTilemapIndex].tiles[selectedTileIndex],
                    worldMousePos.x - tileSize / 2,
                    worldMousePos.y - tileSize / 2, Fade(WHITE, 0.5f)); // Apply transparency
    }
    else if (selectedSpriteIndex >= 0 && selectedSpriteIndex < manager.spriteCount)
    {
        // Display the selected sprite
        DrawTexture(manager.sprites[selectedSpriteIndex].texture,
                    worldMousePos.x - tileSize / 2,
                    worldMousePos.y - tileSize / 2, Fade(WHITE, 0.5f)); // Apply transparency
    }
    else if (selectedAnimationIndex >= 0)
    {
        Animation *selectedAnim = &manager.animations[selectedAnimationIndex];
        DrawTextureRec(selectedAnim->texture, selectedAnim->frames[selectedAnim->currentFrame],
                      (Vector2){worldMousePos.x - tileSize / 2, worldMousePos.y - tileSize / 2}, Fade(WHITE, 0.5f)); // Apply transparency
    }

    // End camera mode
    EndMode2D();

    // Begin GUI rendering (screen space)

    // Draw the tile selection menu for the current tilemap
    const int tileColumns = manager.tilemap[selectedTilemapIndex].tileCountX;
    const int tileRows = (manager.tilemap[selectedTilemapIndex].totalTiles + tileColumns - 1) / tileColumns;

    int selectionGridX = 10; // Fixed position on the screen
    int selectionGridY = 100; // Fixed position on the screen
    DrawRectangle(selectionGridX, selectionGridY, tileColumns * tileSize, tileRows * tileSize, (Color){100, 100, 100, 255});

    // Draw tiles in selection grid
    for (int i = 0; i < manager.tilemap[selectedTilemapIndex].totalTiles; i++)
    {
        int tileX = i % tileColumns;
        int tileY = i / tileColumns;
        DrawTexture(manager.tilemap[selectedTilemapIndex].tiles[i],
                    selectionGridX + tileX * tileSize,
                    selectionGridY + tileY * tileSize, WHITE);
    }

    // Draw Save button
    DrawRectangleRec(saveButton, LIGHTGRAY);
    DrawText("Save Map", saveButton.x + 10, saveButton.y + 5, 20, BLACK);

    // Draw Load button
    DrawRectangleRec(loadButton, LIGHTGRAY);
    DrawText("Load Map", loadButton.x + 10, loadButton.y + 5, 20, BLACK);

    // Show collidability state in GUI (optional)
    const char *collidabilityText = isTileCollidable ? "Collidability: ON" : "Collidability: OFF";
    DrawText(collidabilityText, 250, 10, 20, isTileCollidable ? GREEN : RED);

    // Optionally, draw the selection grid and other GUI elements here
}

void UnloadTilePlacementScene()
{
    FreeTileData(); // Utilize the existing function to free all tile data
}