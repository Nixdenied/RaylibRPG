#include "tile_placement_scene.h"
#include "tilemap.h"
#include "asset_manager.h"
#include <stdlib.h>

static Vector2 mousePosition;
static int selectedTileIndex = 0;
static int selectedSpriteIndex = 0;
static int selectedTilemapIndex = 0;
static const int tileSize = 64;
static int screenTilesX;
static int screenTilesY;

typedef struct TileStack
{
    int *tiles;   // Array of tile indices
    int count;    // Number of tiles in the stack
    int capacity; // Capacity of the tile stack (for dynamic allocation)
} TileStack;

static TileStack **placedTiles;

void InitTilePlacementScene()
{
    InitAssetManager(&manager);
    LoadAssetsFromDirectory(&manager, ASSET_PATH);

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

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    screenTilesX = screenWidth / tileSize;
    screenTilesY = screenHeight / tileSize;

    placedTiles = (TileStack **)malloc(screenTilesY * sizeof(TileStack *));

    for (int i = 0; i < screenTilesY; i++)
    {
        placedTiles[i] = (TileStack *)malloc(screenTilesX * sizeof(TileStack));
        for (int j = 0; j < screenTilesX; j++)
        {
            placedTiles[i][j].tiles = NULL;
            placedTiles[i][j].count = 0;
            placedTiles[i][j].capacity = 0;
        }
    }
}

void PushTileToStack(TileStack *stack, int tileIndex)
{
    if (stack->count >= stack->capacity)
    {
        stack->capacity = (stack->capacity == 0) ? 4 : stack->capacity * 2; // Grow the stack capacity
        stack->tiles = (int *)realloc(stack->tiles, stack->capacity * sizeof(int));
    }
    stack->tiles[stack->count++] = tileIndex; // Add the new tile on top
}

void UpdateTilePlacementScene(float deltaTime)
{
    mousePosition = GetMousePosition();

    int tileX = (int)(mousePosition.x / tileSize);
    int tileY = (int)(mousePosition.y / tileSize);

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
                PushTileToStack(&placedTiles[tileY][tileX], tileIndex);
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
    ClearBackground(RAYWHITE);

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
    UnloadTilemap();

    for (int i = 0; i < screenTilesY; i++)
    {
        free(placedTiles[i]);
    }
    free(placedTiles);
}
