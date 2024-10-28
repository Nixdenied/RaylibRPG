// custom_cursor.c

#include "custom_cursor.h"

// Global variables for cursor textures and state
static Texture2D cursorDefault;
static Texture2D cursorHover;
static Texture2D *currentCursor;  // Pointer to the current cursor texture

void InitCustomCursor(AssetManager *manager)
{
    cursorDefault = GetSprite(manager, "mouse").texture;  // Default pointer
    cursorHover = GetSprite(manager, "select").texture;    // Hover pointer
    currentCursor = &cursorDefault;                    // Start with the default cursor

    HideCursor(); // Hide the system cursor
}

void UpdateCustomCursor(NPC *npcs, int npcCount, Building *buildings, int buildingCount)
{
    Vector2 mousePosition = GetMousePosition();
    bool isHovering = false;

    // Check if the mouse is hovering over any NPC
    for (int i = 0; i < npcCount; i++)
    {
        float radius = npcs[i].animation.frameWidth / 2.0f; // Assuming circular NPCs
        if (CheckCollisionPointCircle(mousePosition, npcs[i].position, radius))
        {
            isHovering = true;
            break;
        }
    }

    // Check if the mouse is hovering over any building
    if (!isHovering)
    {
        for (int i = 0; i < buildingCount; i++)
        {
            float width = buildings[i].completedSprite.texture.width;
            float height = buildings[i].completedSprite.texture.height;
            Rectangle buildingRect = {
                buildings[i].position.x - width / 2,
                buildings[i].position.y - height / 2,
                width,
                height
            };

            if (CheckCollisionPointRec(mousePosition, buildingRect))
            {
                isHovering = true;
                break;
            }
        }
    }

    // Change the cursor based on hover status
    currentCursor = isHovering ? &cursorHover : &cursorDefault;
}

void DrawCustomCursor()
{
    Vector2 mousePosition = GetMousePosition();

    // Adjust the cursor position so the pointer's "hotspot" is at its center
    int offsetX = currentCursor->width / 2;
    int offsetY = currentCursor->height / 2;
    DrawTexture(*currentCursor, mousePosition.x - offsetX, mousePosition.y - offsetY, WHITE);
}

void UnloadCustomCursor()
{
    // No need to unload textures, as they are managed by the asset manager
    ShowCursor(); // Show the system cursor when done
}
