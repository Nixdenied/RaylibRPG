#include "tile_placement_scene.h"
#include "tilemap.h"
#include "asset_manager.h"
#include <stdlib.h>
#include <dirent.h>
#include "tile_placement_data.h"
#include "raylib_utils.h"
#include <stdbool.h>

static Vector2 mousePosition;
bool isTileCollidable = false; // Global collidability state

void InitTilePlacementScene()
{
    InitAssetManager(&manager);
    LoadAssetsFromDirectory(&manager, ASSET_PATH);
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    InitTileData(screenWidth, screenHeight);

    // Log tile information
    if (manager.tilemap && manager.tilemap->tiles)
    {
        for (int i = 0; i < manager.tilemapCount; i++)
        {
            int totalTiles = manager.tilemap[i].totalTiles;
            if (totalTiles > 0)
            {
                printf("Loaded %d tiles.\n", totalTiles);
            }
            else
            {
                printf("No tiles loaded in manager.tilemap->tiles.\n");
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
    mousePosition = GetMousePosition();

    int tileX = (int)(mousePosition.x / tileSize);
    int tileY = (int)(mousePosition.y / tileSize);

    // Detect if the Load button is clicked
    if (CheckCollisionPointRec(mousePosition, loadButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        LoadFirstMapInDirectory("../maps");
        printf("First map loaded from ../maps\n");
    }

    // Detect if the Save button is clicked
    if (CheckCollisionPointRec(mousePosition, saveButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        SaveTilePlacement("../maps/map1.dat");
        printf("Map saved to ../maps/map1.dat\n");
    }

    if (tileX >= 0 && tileX < screenTilesX && tileY >= 0 && tileY < screenTilesY)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            int tileIndex = -1;

            // Check if we're placing a tile
            if (selectedTileIndex >= 0 && selectedTileIndex < manager.tilemap[selectedTilemapIndex].totalTiles)
            {
                tileIndex = (selectedTilemapIndex * 1000) + selectedTileIndex;
            }
            // Check if we're placing a sprite
            else if (selectedSpriteIndex >= 0 && selectedSpriteIndex < manager.spriteCount)
            {
                tileIndex = (selectedTilemapIndex * 1000) + manager.tilemap[selectedTilemapIndex].totalTiles + selectedSpriteIndex;
            }

            // Push to the stack if valid
            if (tileIndex != -1)
            {
                PushTileToStack(&placedTiles[tileY][tileX], tileIndex, isTileCollidable);
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            // Remove the top tile from the stack if it exists
            if (placedTiles[tileY][tileX].count > 0)
            {
                placedTiles[tileY][tileX].count--;
            }
        }
        // Toggle collidability with the "C" key
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
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            selectedTileIndex = (selectedTileIndex + 1) % manager.tilemap[selectedTilemapIndex].totalTiles;
            selectedSpriteIndex = -1; // Deactivate sprite selection when switching to tiles
        }

        // Change selected sprite with UP/DOWN keys
        if (IsKeyPressed(KEY_UP))
        {
            selectedSpriteIndex = (selectedSpriteIndex - 1 + manager.spriteCount) % manager.spriteCount;
            selectedTileIndex = -1; // Deactivate tile selection when switching to sprites
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            selectedSpriteIndex = (selectedSpriteIndex + 1) % manager.spriteCount;
            selectedTileIndex = -1; // Deactivate tile selection when switching to sprites
        }

        // Switch between tilemaps with PAGE_UP and PAGE_DOWN
        if (IsKeyPressed(KEY_PAGE_UP))
        {
            selectedTilemapIndex = (selectedTilemapIndex - 1 + manager.tilemapCount) % manager.tilemapCount;
        }
        if (IsKeyPressed(KEY_PAGE_DOWN))
        {
            selectedTilemapIndex = (selectedTilemapIndex + 1) % manager.tilemapCount;
        }
    }
}

void RenderTilePlacementScene()
{
    ClearBackground(GetColorFromHex("#47aaa9"));

    // Draw Save button
    DrawRectangleRec(saveButton, LIGHTGRAY);
    DrawText("Save Map", saveButton.x + 10, saveButton.y + 10, 10, BLACK);

    // Draw Load button
    DrawRectangleRec(loadButton, LIGHTGRAY);
    DrawText("Load Map", loadButton.x + 10, loadButton.y + 10, 10, BLACK);

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

    // Draw the grid lines
    for (int x = 0; x <= screenTilesX; x++)
    {
        DrawLine(x * tileSize, 0, x * tileSize, screenTilesY * tileSize, BLACK);
    }
    for (int y = 0; y <= screenTilesY; y++)
    {
        DrawLine(0, y * tileSize, screenTilesX * tileSize, y * tileSize, BLACK);
    }

    // Draw the tile selection menu at the bottom of the screen for the current tilemap
    const int tileColumns = manager.tilemap[selectedTilemapIndex].tileCountX;
    const int tileRows = (manager.tilemap[selectedTilemapIndex].totalTiles + tileColumns - 1) / tileColumns;

    int selectionGridX = 0;
    int selectionGridY = GetScreenHeight() - (tileRows * tileSize);
    DrawRectangle(selectionGridX, selectionGridY, tileColumns * tileSize, tileRows * tileSize, (Color){100, 100, 100, 255});

    // Show collidability state
    const char *collidabilityText = isTileCollidable ? "Collidability: ON" : "Collidability: OFF";
    DrawText(collidabilityText, 250, 10, 24, isTileCollidable ? GREEN : RED);

    for (int i = 0; i < manager.tilemap[selectedTilemapIndex].totalTiles; i++)
    {
        int tileX = i % tileColumns;
        int tileY = i / tileColumns;
        DrawTexture(manager.tilemap[selectedTilemapIndex].tiles[i], selectionGridX + tileX * tileSize, selectionGridY + tileY * tileSize, WHITE);
    }

    // Display the currently selected tile or sprite at the mouse position
    if (selectedTileIndex >= 0 && selectedTileIndex < manager.tilemap[selectedTilemapIndex].totalTiles)
    {
        // Display the selected tile from the currently selected tilemap
        DrawTexture(manager.tilemap[selectedTilemapIndex].tiles[selectedTileIndex], mousePosition.x - tileSize / 2, mousePosition.y - tileSize / 2, WHITE);
    }
    else if (selectedSpriteIndex >= 0 && selectedSpriteIndex < manager.spriteCount)
    {
        // Display the selected sprite
        DrawTexture(manager.sprites[selectedSpriteIndex].texture, mousePosition.x - tileSize / 2, mousePosition.y - tileSize / 2, WHITE);
    }
}

void UnloadTilePlacementScene()
{
    if (placedTiles != NULL)
    {
        // Free each row of TileStack
        for (int i = 0; i < screenTilesY; i++)
        {
            if (placedTiles[i] != NULL)
            {
                // Free any dynamically allocated tiles within each TileStack
                for (int j = 0; j < screenTilesX; j++)
                {
                    if (placedTiles[i][j].tiles != NULL)
                    {
                        free(placedTiles[i][j].tiles);  // Free the tile array
                        placedTiles[i][j].tiles = NULL; // Reset pointer
                    }
                }
                free(placedTiles[i]); // Free the row itself
            }
        }
        free(placedTiles); // Free the top-level pointer
        placedTiles = NULL;
    }
}
