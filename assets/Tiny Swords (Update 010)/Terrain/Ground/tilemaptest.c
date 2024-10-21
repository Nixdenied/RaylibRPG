#include "raylib.h"

#define TILE_SIZE 64
#define TILE_COUNT_X 10 // Number of tiles horizontally
#define TILE_COUNT_Y 10 // Number of tiles vertically
#define TOTAL_TILES (TILE_COUNT_X * TILE_COUNT_Y)

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Tilemap Example");

    // Load tilemap texture
    Texture2D tilemap = LoadTexture("TilemapFlat.png");

    // Create an array to hold the tiles
    Texture2D tiles[TOTAL_TILES];

    // Load the entire texture as an image
    Image tilemapImage = LoadImageFromTexture(tilemap);

    // Cut the tilemap into 64x64 tiles
    for (int y = 0; y < TILE_COUNT_Y; y++)
    {
        for (int x = 0; x < TILE_COUNT_X; x++)
        {
            Rectangle srcRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

            // Create a copy of the tilemap image for cropping
            Image tileImage = ImageCopy(tilemapImage); // Make a copy of the original image

            // Crop the copied image into the tile
            ImageCrop(&tileImage, srcRect);

            // Load the cropped image as a texture
            tiles[y * TILE_COUNT_X + x] = LoadTextureFromImage(tileImage);

            // Unload the cropped image
            UnloadImage(tileImage);
        }
    }

    // Unload the original tilemap image
    UnloadImage(tilemapImage);

    // Index for the currently selected tile
    int currentTileIndex = 0;

    // Main game loop
    while (!WindowShouldClose())
    {
        // Input handling
        if (IsKeyPressed(KEY_RIGHT))
        {
            currentTileIndex++;
            if (currentTileIndex >= TOTAL_TILES)
            {
                currentTileIndex = 0; // Wrap around to the first tile
            }
        }
        if (IsKeyPressed(KEY_LEFT))
        {
            currentTileIndex--;
            if (currentTileIndex < 0)
            {
                currentTileIndex = TOTAL_TILES - 1; // Wrap around to the last tile
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the currently selected tile
        DrawTexture(tiles[currentTileIndex], 100, 100, WHITE);

        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < TOTAL_TILES; i++)
    {
        UnloadTexture(tiles[i]);
    }
    UnloadTexture(tilemap);
    CloseWindow();

    return 0;
}
