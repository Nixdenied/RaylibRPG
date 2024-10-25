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

Vector2 squarePosition = {600, 400}; // Initial position of the square
const float squareSpeed = 200.0f;    // Speed of the square
Rectangle squareBounds;              // Bounds of the square

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
    InitTileData(screenWidth, screenHeight);
    LoadFirstMapInDirectory("../maps"); // Load a map from your specified directory
}

void UpdateTestMapScene(float deltaTime)
{
    squareBounds = (Rectangle){squarePosition.x, squarePosition.y, 50, 50}; // Update square bounds

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
}

void RenderTestMapScene()
{
    ClearBackground(BLACK);

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

    // Draw the controllable square
    DrawRectangle(squarePosition.x, squarePosition.y, 50, 50, BLUE); // A blue square of 50x50
}