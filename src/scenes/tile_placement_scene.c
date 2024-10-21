#include "tile_placement_scene.h"
#include "tilemap.h"
#include <stdlib.h>

static Vector2 mousePosition;     // Mouse position for tile placement
static int selectedTileIndex = 0; // Currently selected tile index
static const int tileSize = 64;   // Size of each tile
static int **placedTiles;         // 2D array to store placed tiles
static int screenTilesX;          // Number of tiles fitting horizontally
static int screenTilesY;          // Number of tiles fitting vertically

void InitTilePlacementScene()
{
    tilemap = LoadTilemap("/opt/git/RayLibRPG/assets/Tiny Swords (Update 010)/Terrain/Ground/TilemapFlat.png", tileSize);

    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calculate how many tiles fit on the screen
    screenTilesX = screenWidth / tileSize;  // Number of tiles horizontally
    screenTilesY = screenHeight / tileSize; // Number of tiles vertically

    // Allocate memory for placed tiles based on the entire screen size
    placedTiles = (int **)malloc(screenTilesY * sizeof(int *));
    for (int i = 0; i < screenTilesY; i++)
    {
        placedTiles[i] = (int *)malloc(screenTilesX * sizeof(int));
        for (int j = 0; j < screenTilesX; j++)
        {
            placedTiles[i][j] = -1; // Initialize with -1 to indicate empty
        }
    }
}

void UpdateTilePlacementScene(float deltaTime)
{
    // Update mouse position
    mousePosition = GetMousePosition();

    // Snap the mouse position to the nearest tile grid
    int tileX = (int)(mousePosition.x / tileSize); // Snap X to nearest tile grid
    int tileY = (int)(mousePosition.y / tileSize); // Snap Y to nearest tile grid

    // Ensure tile indices are within the bounds of the full screen grid
    if (tileX >= 0 && tileX < screenTilesX && tileY >= 0 && tileY < screenTilesY)
    {
        // Check for left-click to place tile
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            placedTiles[tileY][tileX] = selectedTileIndex; // Store the selected tile index
        }

        // Check for right-click to delete tile
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            placedTiles[tileY][tileX] = -1; // Remove the tile by setting it to -1
        }
    }

    // Tile selection with keyboard input (up/down arrows)
    if (IsKeyPressed(KEY_UP))
    {
        selectedTileIndex = (selectedTileIndex - 1 + tilemap.totalTiles) % tilemap.totalTiles; // Cycle through tiles
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        selectedTileIndex = (selectedTileIndex + 1) % tilemap.totalTiles; // Cycle through tiles
    }
}

void RenderTilePlacementScene()
{
    ClearBackground(RAYWHITE);

    // Draw the placed tiles on the main grid
    for (int y = 0; y < screenTilesY; y++)
    {
        for (int x = 0; x < screenTilesX; x++)
        {
            if (placedTiles[y][x] != -1)
            {
                DrawTexture(tilemap.tiles[placedTiles[y][x]], x * tileSize, y * tileSize, WHITE);
            }
        }
    }

    // Draw the grid lines
    for (int x = 0; x <= screenTilesX; x++) // Draw vertical lines
    {
        DrawLine(x * tileSize, 0, x * tileSize, screenTilesY * tileSize, BLACK);
    }
    for (int y = 0; y <= screenTilesY; y++) // Draw horizontal lines
    {
        DrawLine(0, y * tileSize, screenTilesX * tileSize, y * tileSize, BLACK);
    }

    // Determine the number of columns and rows for the tile selection grid
    const int tileColumns = tilemap.tileCountX;                                // Number of columns to display tiles in the selection area
    const int tileRows = (tilemap.totalTiles + tileColumns - 1) / tileColumns; // Total number of rows needed

    // Set the starting position of the tile selection grid (bottom of the screen)
    int selectionGridX = 0;
    int selectionGridY = GetScreenHeight() - (tileRows * tileSize);

    // Draw a background for the tile selection area
    DrawRectangle(selectionGridX, selectionGridY, tileColumns * tileSize, tileRows * tileSize, (Color){100, 100, 100, 255});

    // Draw the available tiles in a grid layout in the selection area
    for (int i = 0; i < tilemap.totalTiles; i++)
    {
        int tileX = i % tileColumns; // Column position
        int tileY = i / tileColumns; // Row position

        // Draw each tile in the grid
        DrawTexture(tilemap.tiles[i], selectionGridX + tileX * tileSize, selectionGridY + tileY * tileSize, WHITE);
    }

    // Optionally show the currently selected tile under the mouse cursor
    if (selectedTileIndex >= 0)
    {
        DrawTexture(tilemap.tiles[selectedTileIndex], mousePosition.x - tileSize / 2, mousePosition.y - tileSize / 2, WHITE);
    }
}

void UnloadTilePlacementScene()
{
    UnloadTilemap(&tilemap);

    // Free allocated memory for placed tiles
    for (int i = 0; i < tilemap.tileCountY; i++)
    {
        free(placedTiles[i]);
    }
    free(placedTiles);
}
