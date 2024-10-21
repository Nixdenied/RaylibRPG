#include "tilemap.h"
#include <stdlib.h>

Tilemap tilemap;

// Load the tilemap from a file and cut it into tiles
Tilemap LoadTilemap(const char *filePath, int tileSize)
{

    // Load the tilemap texture as an image
    Texture2D tilemapTexture = LoadTexture(filePath);
    Image tilemapImage = LoadImageFromTexture(tilemapTexture);

    // Calculate tile count based on image size and tile size
    tilemap.tileCountX = tilemapImage.width / tileSize;
    tilemap.tileCountY = tilemapImage.height / tileSize;
    tilemap.totalTiles = tilemap.tileCountX * tilemap.tileCountY;
    tilemap.currentTileIndex = 0;

    // Allocate memory for the tiles array
    tilemap.tiles = (Texture2D *)malloc(tilemap.totalTiles * sizeof(Texture2D));

    // Cut the tilemap into tiles
    for (int y = 0; y < tilemap.tileCountY; y++)
    {
        for (int x = 0; x < tilemap.tileCountX; x++)
        {
            // Ensure srcRect does not exceed the bounds of the image
            Rectangle srcRect = {x * tileSize, y * tileSize, tileSize, tileSize};

            // Check if we can crop this tile (within bounds)
            if (srcRect.x + srcRect.width <= tilemapImage.width &&
                srcRect.y + srcRect.height <= tilemapImage.height)
            {
                Image tileImage = ImageCopy(tilemapImage); // Make a copy of the original image
                ImageCrop(&tileImage, srcRect);            // Crop the image to the tile

                // Load the cropped image as a texture
                tilemap.tiles[y * tilemap.tileCountX + x] = LoadTextureFromImage(tileImage);

                // Check if the texture was loaded successfully
                if (tilemap.tiles[y * tilemap.tileCountX + x].id == 0)
                {
                    // Handle texture loading failure (optional logging)
                }

                // Unload the cropped image
                UnloadImage(tileImage);
            }
        }
    }

    // Unload the original tilemap image
    UnloadImage(tilemapImage);
    UnloadTexture(tilemapTexture);

    return tilemap;
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
void UnloadTilemap(Tilemap *tilemap)
{
    for (int i = 0; i < tilemap->totalTiles; i++)
    {
        UnloadTexture(tilemap->tiles[i]);
    }
    free(tilemap->tiles);
}