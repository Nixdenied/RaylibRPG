#include <stdlib.h>
#include "tilemap.h"
#include "asset_manager.h"
#include <math.h>

// Load the tilemap from a file and cut it into tiles
void LoadTilemap(const char *filePath, int tileSize) {
    // Load the tilemap texture as an image
    Texture2D tilemapTexture = LoadTexture(filePath);
    Image tilemapImage = LoadImageFromTexture(tilemapTexture);

    // Calculate tile count based on image size and tile size
    int tileCountX = tilemapImage.width / tileSize;
    int tileCountY = tilemapImage.height / tileSize;
    int totalTiles = tileCountX * tileCountY;

    // Allocate memory for the tiles array
    Texture2D *tiles = (Texture2D *)malloc(totalTiles * sizeof(Texture2D));

    // Cut the tilemap into tiles and store them in the asset manager
    for (int y = 0; y < tileCountY; y++) {
        for (int x = 0; x < tileCountX; x++) {
            Rectangle srcRect = { x * tileSize, y * tileSize, tileSize, tileSize };

            if (srcRect.x + srcRect.width <= tilemapImage.width &&
                srcRect.y + srcRect.height <= tilemapImage.height) {
                
                Image tileImage = ImageCopy(tilemapImage);
                ImageCrop(&tileImage, srcRect);

                Texture2D tileTexture = LoadTextureFromImage(tileImage);
                if (tileTexture.id != 0) {
                    tiles[y * tileCountX + x] = tileTexture; // Store the texture

                    // Create a unique name for this tile (e.g., "tile_X_Y")
                    char tileName[64];
                    snprintf(tileName, sizeof(tileName), "tile_%d_%d", x, y);

                    // Add the tile to the asset manager
                    Sprite tileSprite = { .texture = tileTexture };

                }

                UnloadImage(tileImage);
            }
        }
    }

    // Prepare the new tilemap to store in the asset manager
    Tilemap newTilemap;
    newTilemap.tiles = tiles; // Assign the allocated tiles array
    newTilemap.tileCountX = tileCountX;
    newTilemap.tileCountY = tileCountY;
    newTilemap.totalTiles = totalTiles;
    newTilemap.currentTileIndex = 0;

    // Add the tilemap to the asset manager
    manager.tilemap[manager.tilemapCount++] = newTilemap; // Increment tilemapCount after assignment


    // Clean up
    UnloadImage(tilemapImage);
    UnloadTexture(tilemapTexture);
}


// Update the tilemap (e.g., switch between tiles with arrow keys)
void UpdateTilemap(Tilemap *tilemap)
{
    if (IsKeyPressed(KEY_RIGHT))
    {
        tilemap->currentTileIndex++;
        if (tilemap->currentTileIndex >= tilemap->totalTiles)
        {
            tilemap->currentTileIndex = 0; // Wrap around to the first tile
        }
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        tilemap->currentTileIndex--;
        if (tilemap->currentTileIndex < 0)
        {
            tilemap->currentTileIndex = tilemap->totalTiles - 1; // Wrap around to the last tile
        }
    }
}

// Unload all textures in the tilemap and free memory
void UnloadTilemap()
{
    for (int i = 0; i < manager.tilemap->totalTiles; i++)
    {
        UnloadTexture(manager.tilemap->tiles[i]);
    }
    free(manager.tilemap->tiles);
}