#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tilemap.h"
#include "asset_manager.h"
#include "raylib_utils.h"

void AddAssetToHashTable(AssetManager *manager, const char *name, Sprite sprite)
{
    unsigned long hash = HashString(name) % HASH_TABLE_SIZE;

    AssetNode *newNode = (AssetNode *)malloc(sizeof(AssetNode));
    strncpy(newNode->name, name, 64);
    newNode->sprite = sprite; // Store the sprite directly
    newNode->next = manager->hashTable[hash];
    manager->hashTable[hash] = newNode;
}

void AddAnimationToHashTable(AssetManager *manager, const char *name, Animation animation)
{
    unsigned long hash = HashString(name) % HASH_TABLE_SIZE;

    AssetNode *newNode = (AssetNode *)malloc(sizeof(AssetNode));
    strncpy(newNode->name, name, 64);
    newNode->animation = animation; // Store the animation directly
    newNode->next = manager->hashTable[hash];
    manager->hashTable[hash] = newNode;
}

Sprite GetSprite(AssetManager *manager, const char *name)
{
    unsigned long hash = HashString(name) % HASH_TABLE_SIZE;
    AssetNode *node = manager->hashTable[hash];

    while (node)
    {
        if (strcmp(node->name, name) == 0)
        {
            return node->sprite; // Return the sprite struct directly
        }
        node = node->next;
    }
    return (Sprite){0}; // Return a default sprite if not found
}

Animation GetAnimation(AssetManager *manager, const char *name)
{
    unsigned long hash = HashString(name) % HASH_TABLE_SIZE;
    AssetNode *node = manager->hashTable[hash];

    while (node)
    {
        if (strcmp(node->name, name) == 0)
        {
            return node->animation; // Return the animation struct directly
        }
        node = node->next;
    }
    return (Animation){0}; // Return a default animation if not found
}

Tilemap GetTilemap(AssetManager *manager, const char *name)
{
    unsigned long hash = HashString(name) % HASH_TABLE_SIZE;
    AssetNode *node = manager->hashTable[hash];

    while (node)
    {
        if (strcmp(node->name, name) == 0)
        {
            return node->tilemap; // Return the tilemap struct directly
        }
        node = node->next;
    }
    return (Tilemap){0}; // Return a default tilemap if not found
}

// Function to parse the filename and extract animation data
// Modified function to parse filename and extract name, rows, framesPerRow, frameWidth, frameHeight
void ParseAnimationInfoFromFilename(const char *filePath, char *name, int *rows, int *framesPerRow, int *frameWidth, int *frameHeight)
{
    const char *fileName = strrchr(filePath, '/'); // Find the last '/' character
    if (fileName)
    {
        fileName++; // Move past the '/'
    }
    else
    {
        fileName = filePath; // If no '/' is found, use the whole path as the file name
    }

    // Remove the extension by finding the last '.' character
    char *dotPos = strrchr(fileName, '.');
    char nameWithoutExtension[256];
    if (dotPos)
    {
        strncpy(nameWithoutExtension, fileName, dotPos - fileName);
        nameWithoutExtension[dotPos - fileName] = '\0'; // Null terminate the string
    }
    else
    {
        strncpy(nameWithoutExtension, fileName, sizeof(nameWithoutExtension));
        nameWithoutExtension[sizeof(nameWithoutExtension) - 1] = '\0'; // Ensure null termination
    }

    // Tokenize the filename using '_' as a delimiter
    char *token = strtok(nameWithoutExtension, "_");
    if (token)
    {
        strncpy(name, token, 64); // Extract the name (first part of the filename)
        name[63] = '\0';          // Ensure the name is null-terminated
    }

    token = strtok(NULL, "_");
    if (token)
        *rows = atoi(token); // Extract the number of rows
    token = strtok(NULL, "_");
    if (token)
        *framesPerRow = atoi(token); // Extract the number of frames per row
    token = strtok(NULL, "_");
    if (token)
        *frameWidth = atoi(token); // Extract the frame width
    token = strtok(NULL, "_");
    if (token)
        *frameHeight = atoi(token); // Extract the frame height
}

// Implementation

void InitAssetManager(AssetManager *manager)
{
    manager->spriteCount = 0;
    manager->animationCount = 0;
    manager->tilemapCount = 0;
}

// Function to check if a frame is blank (more than 90% transparent pixels)
bool IsFrameBlank(Texture2D texture, Rectangle frame)
{
    // Load pixel data of the entire texture
    Image image = LoadImageFromTexture(texture); // Load the texture as an image
    Color *pixels = LoadImageColors(image);      // Get the color data for each pixel

    int transparentPixelCount = 0;
    int totalPixelCount = frame.width * frame.height;

    // Iterate through the pixels of the frame
    for (int y = frame.y; y < frame.y + frame.height; y++)
    {
        for (int x = frame.x; x < frame.x + frame.width; x++)
        {
            Color pixel = pixels[y * texture.width + x];
            if (pixel.a <= TRANSPARENCY_THRESHOLD * 255)
            {
                transparentPixelCount++;
            }
        }
    }

    // Free the image and pixel data
    UnloadImage(image);
    UnloadImageColors(pixels);

    // If more than MAX_TRANSPARENT_PIXELS percentage of pixels are transparent, return true (frame is blank)
    float transparentRatio = (float)transparentPixelCount / totalPixelCount;
    return (transparentRatio >= MAX_TRANSPARENT_PIXELS);
}

void LoadSprite(AssetManager *manager, const char *filePath)
{
    if (manager->spriteCount < MAX_SPRITES)
    {
        manager->sprites[manager->spriteCount].texture = LoadTexture(filePath);

        // Extract the name from the filename
        char name[64];
        ParseAnimationInfoFromFilename(filePath, name, NULL, NULL, NULL, NULL); // We only need the name part here
        strncpy(manager->sprites[manager->spriteCount].name, name, 64);
        manager->sprites[manager->spriteCount].drawName = true; // Set this flag as needed

        // Add the loaded sprite to the hash table for fast lookup
        AddAssetToHashTable(manager, name, manager->sprites[manager->spriteCount]);

        manager->spriteCount++;
    }
    else
    {
        printf("Max sprites loaded!\n");
    }
}

void LoadAnimation(AssetManager *manager, const char *filePath)
{
    if (manager->animationCount < MAX_ANIMATIONS)
    {
        manager->animations[manager->animationCount].texture = LoadTexture(filePath);

        // Variables to hold parsed information
        int rows, framesPerRow, frameWidth, frameHeight;
        char name[64]; // Variable to hold the name

        // Parse the filename to get name, rows, framesPerRow, frameWidth, frameHeight
        ParseAnimationInfoFromFilename(filePath, name, &rows, &framesPerRow, &frameWidth, &frameHeight);

        // Copy the name and set the flag for drawing the name
        strncpy(manager->animations[manager->animationCount].name, name, 64);
        manager->animations[manager->animationCount].drawName = true; // Set this to true or false as needed

        // The rest of the logic for loading animations...
        manager->animations[manager->animationCount].frameWidth = frameWidth;
        manager->animations[manager->animationCount].frameHeight = frameHeight;
        manager->animations[manager->animationCount].rows = rows;
        manager->animations[manager->animationCount].framesPerRow = framesPerRow;
        manager->animations[manager->animationCount].frameCount = rows * framesPerRow;

        // Allocate memory for the frame rectangles
        manager->animations[manager->animationCount].frames =
            (Rectangle *)malloc(manager->animations[manager->animationCount].frameCount * sizeof(Rectangle));

        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < framesPerRow; x++)
            {
                int index = y * framesPerRow + x;
                manager->animations[manager->animationCount].frames[index] = (Rectangle){
                    x * frameWidth,
                    y * frameHeight,
                    frameWidth,
                    frameHeight};
            }
        }

        // Add the loaded animation to the hash table for fast lookup
        AddAnimationToHashTable(manager, name, manager->animations[manager->animationCount]);

        manager->animations[manager->animationCount].currentFrame = 0;
        manager->animations[manager->animationCount].frameTime = 0.1f; // 100 ms
        manager->animations[manager->animationCount].elapsedTime = 0.0f;

        manager->animationCount++;
    }
    else
    {
        printf("Max animations loaded!\n");
    }
}

void LoadAssetsFromDirectory(AssetManager *manager, const char *directory)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            // Ignore the current and parent directory entries
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
                char filePath[256];
                snprintf(filePath, sizeof(filePath), "%s/%s", directory, ent->d_name);

                // Check if the entry is a directory
                struct stat pathStat;
                stat(filePath, &pathStat);
                if (S_ISDIR(pathStat.st_mode))
                {
                    // If it's a directory, recursively call this function
                    LoadAssetsFromDirectory(manager, filePath);
                }
                else
                {
                    // Load assets based on their file extensions
                    if (strstr(ent->d_name, ".png") != NULL)
                    {
                        // Determine if it's a static sprite or an animation based on filename pattern
                        if (strstr(ent->d_name, "Tilemap") != NULL)
                        {
                            LoadTilemap(filePath, 64);
                        }
                        else if (strstr(ent->d_name, "_") != NULL)
                        {
                            LoadAnimation(manager, filePath); // Animation files have underscores in their names
                        }
                        else
                        {
                            LoadSprite(manager, filePath); // Static images do not
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    else
    {
        perror("Could not open assets directory");
    }
}

void UpdateAnimations(AssetManager *manager, float deltaTime)
{
    for (int i = 0; i < manager->animationCount; i++)
    {
        Animation *anim = &manager->animations[i];
        anim->elapsedTime += deltaTime;
        if (anim->elapsedTime >= anim->frameTime)
        {
            anim->elapsedTime -= anim->frameTime;
            anim->currentFrame++;
            if (anim->currentFrame >= anim->frameCount)
            {
                anim->currentFrame = 0; // Loop the animation
            }
        }
    }
}

void UnloadAssets(AssetManager *manager)
{
    for (int i = 0; i < manager->spriteCount; i++)
    {
        UnloadTexture(manager->sprites[i].texture);
    }
    for (int i = 0; i < manager->animationCount; i++)
    {
        UnloadTexture(manager->animations[i].texture);
        free(manager->animations[i].frames); // Free the frames array
    }
}