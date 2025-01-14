// npc.c

#include "npc.h"
#include "raylib.h"
#include "raymath.h"
#include "asset_manager.h"
#include <stdio.h>
#include <math.h>
#include "debug.h"

int npcCount = 0;

/**
 * @brief Initializes an NPC with the given position, speed, and initial animation.
 *
 * @param npc Pointer to the NPC to initialize.
 * @param manager Pointer to the AssetManager.
 * @param position Initial position of the NPC.
 * @param speed Movement speed of the NPC (pixels per second).
 * @param initialAnimationName Name of the initial animation to assign.
 */
void InitNPC(NPC *npc, AssetManager *manager, Vector2 position, float speed, const char *initialAnimationName)
{
    npc->position = position;
    npc->targetPosition = position;
    npc->speed = speed;
    npc->state = NPC_IDLE;
    npc->health = 100.0f;
    npc->strength = 10;
    npc->defense = 5;
    npc->unitType = initialAnimationName;

    npc->isCollidable = true;
    npc->drawName = false;
    npc->isSelected = false;

    npc->collisionRadius = 30.0f; // Set collision radius (adjust as necessary)
    npc->separationForce = 50.0f; // Set separation force strength (adjust as needed)

    Animation initialAnimation = GetAnimation(manager, initialAnimationName);
    if (initialAnimation.frameCount > 0)
    {
        npc->animation = initialAnimation;
        npc->animation.currentFrame = 0;
        npc->animation.elapsedTime = 0.0f;
        // Initialize bounding box
        npc->boundingBox = (Rectangle){
            position.x - initialAnimation.frameWidth / 8,
            position.y - initialAnimation.frameHeight / 8,
            initialAnimation.frameWidth / 4,
            initialAnimation.frameHeight / 4};
    }
    else
    {
        fprintf(stderr, "Error: Animation '%s' not found or invalid.\n", initialAnimationName);
        npc->animation = (Animation){0};
    }
}

void UpdateNPC(NPC *npc, NPC npcs[], float deltaTime)
{
    Vector2 separation = {0.0f, 0.0f};
    int neighbors = 0;

    // Calculate separation force from nearby NPCs
    for (int i = 0; i < npcCount; i++)
    {
        if (&npcs[i] != npc) // Ensure we’re not calculating separation against itself
        {
            float distance = Vector2Distance(npc->position, npcs[i].position);
            if (distance < npc->collisionRadius * 2.0f) // Check if within collision radius
            {
                Vector2 away = Vector2Subtract(npc->position, npcs[i].position);
                away = Vector2Scale(Vector2Normalize(away), 1.0f / (distance + 0.01f)); // Weight by inverse distance
                separation = Vector2Add(separation, away);
                neighbors++;
            }
        }
    }

    // Update bounding box
    npc->boundingBox = (Rectangle){
        npc->position.x - npc->animation.frameWidth / 8,
        npc->position.y - npc->animation.frameHeight / 8,
        npc->animation.frameWidth / 4,
        npc->animation.frameHeight / 4};

    // Apply separation force if there are nearby NPCs
    if (neighbors > 0)
    {
        separation = Vector2Scale(separation, npc->separationForce); // Scale by separation force
        npc->position = Vector2Add(npc->position, Vector2Scale(separation, deltaTime));
    }

    switch (npc->state)
    {
    case NPC_IDLE:
        // Idle behavior (e.g., stand still)
        break;
    case NPC_WALKING:
    {
        Vector2 direction = Vector2Subtract(npc->targetPosition, npc->position);
        float distance = Vector2Length(direction);

        if (distance > 1.0f)
        { // Threshold to stop moving
            Vector2 directionNormalized = Vector2Scale(direction, 1.0f / distance);
            Vector2 movement = Vector2Scale(directionNormalized, npc->speed * deltaTime);

            if (Vector2Length(movement) > distance)
            {
                npc->position = npc->targetPosition;
                SetNPCState(npc, NPC_IDLE); // Change to idle when target reached
            }
            else
            {
                npc->position = Vector2Add(npc->position, movement);
            }

            UpdateAnimation(&npc->animation); // Update animation while moving
        }
        else
        {
            SetNPCState(npc, NPC_IDLE); // Change to idle if close enough to stop
        }
        break;
    }
    case NPC_TALKING:
        // Implement talking behavior
        break;
    case NPC_ATTACKING:
        // Implement attacking behavior
        break;
    case NPC_DEAD:
        // Implement dead behavior
        break;
    default:
        break;
    }
}

/**
 * @brief Updates the NPC's current animation based on elapsed time.
 *
 * @param animation Pointer to the Animation to update.
 */
void UpdateAnimation(Animation *animation)
{
    if (animation->frameCount <= 0)
        return; // Safety check

    animation->elapsedTime += GetFrameTime();

    if (animation->elapsedTime >= animation->frameTime)
    {
        animation->currentFrame++;
        if (animation->currentFrame >= animation->frameCount)
        {
            animation->currentFrame = 0;
        }
        animation->elapsedTime = 0.0f;
    }
}

/**
 * @brief Draws the NPC on the screen, including its current animation frame and optional name.
 *
 * @param npc Pointer to the NPC to draw.
 */
void DrawNPC(NPC *npc)
{
    // If selected, draw a smaller selection circle
    if (npc->isSelected)
    {
        for (int offset = 1; offset <= 3; offset++)
        {
            DrawEllipseLines(
                npc->position.x,                                  // Center X
                npc->position.y + npc->animation.frameHeight / 5, // Center Y (slightly below the NPC)
                npc->animation.frameWidth / 6 + offset,           // Horizontal radius
                npc->animation.frameHeight / 12 + offset,         // Vertical radius
                GREEN                                             // Color
            );
        } // Draw 3 ellipses with offset for thicker line
    }
    // Draw the current frame of the animation at the NPC's position
    if (npc->animation.frameCount > 0 && npc->animation.texture.id != 0)
    { // Check if animation is valid
        Rectangle frame = npc->animation.frames[npc->animation.currentFrame];
        // Center the texture on the NPC's position
        Vector2 drawPosition = Vector2Subtract(npc->position, (Vector2){frame.width / 2, frame.height / 2});
        DrawTextureRec(npc->animation.texture, frame, drawPosition, WHITE);
    }

    // Draw collision radius for debugging
    if (DEBUG)
    {
        DrawCircleLines(npc->position.x, npc->position.y, npc->collisionRadius, RED);
    }

    // Optionally draw the NPC's name above it
    if (npc->drawName && strlen(npc->animation.name) > 0)
    {
        int textWidth = MeasureText(npc->animation.name, 10);
        Vector2 textPosition = {npc->position.x - textWidth / 2, npc->position.y - 25};
        DrawText(npc->animation.name, textPosition.x, textPosition.y, 10, RAYWHITE);
    }

    if (npc->isSelected && DEBUG)
    {
        DrawRectangleLinesEx(npc->boundingBox, 2, GREEN); // Draw bounding box
    }
}

/**
 * @brief Sets the NPC's state and updates its animation accordingly.
 *
 * @param npc Pointer to the NPC.
 * @param newState The new state to assign to the NPC.
 */
void SetNPCState(NPC *npc, NPCState newState)
{
    if (npc->state != newState)
    {
        npc->state = newState;

        // Define the suffix based on the state
        const char *suffix;
        switch (npc->state)
        {
        case NPC_IDLE:
            suffix = "_1"; // _1 for idle
            break;
        case NPC_WALKING:
            suffix = "_2"; // _2 for walking
            break;
        case NPC_ATTACKING:
            suffix = "_3"; // _3 for attacking
            break;
        default:
            return;
        }

        // Create a base name by removing any existing suffix
        char baseName[64];
        snprintf(baseName, sizeof(baseName), "%s", npc->unitType);

        // Remove any existing suffix by finding the last underscore and truncating
        char *underscorePos = strrchr(baseName, '_');
        if (underscorePos != NULL)
        {
            *underscorePos = '\0'; // Truncate at last underscore
        }

        // Construct the animation name with the new suffix
        char animationName[64];
        snprintf(animationName, sizeof(animationName), "%s%s", baseName, suffix);

        // Retrieve and assign the animation
        Animation animation = GetAnimation(&manager, animationName);
        if (animation.frameCount > 0)
        {
            npc->animation = animation;
            npc->animation.currentFrame = 0;
            npc->animation.elapsedTime = 0.0f;
        }
        else
        {
            fprintf(stderr, "Error: Animation '%s' not found or invalid.\n", animationName);
        }
    }
}

/**
 * @brief Processes mouse input to handle NPC selection and movement.
 *
 * @param npcs Array of NPCs.
 * @param npcCount Number of NPCs in the array.
 * @param mousePosition Current mouse position.
 * @param mousePressed Boolean indicating if the mouse was pressed.
 */
void HandleNPCMouseInput(NPC *npcs, int npcCount, Vector2 mousePosition, bool mousePressed)
{
    bool clickedOnNPC = false;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        // Deselect all NPCs on right-click
        for (int i = 0; i < npcCount; i++)
        {
            npcs[i].isSelected = false;
        }
    }
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // Check if Shift is held down
        bool shiftHeld = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        
        // Iterate in reverse to prioritize topmost NPCs if overlapping
        for (int i = npcCount - 1; i >= 0; i--)
        {
            Animation currentAnimation = npcs[i].animation;
            float radius = currentAnimation.frameWidth / 4; // Assuming circular NPCs

            if (CheckCollisionPointRec(mousePosition, npcs[i].boundingBox))
            {
                clickedOnNPC = true;
                
                // If Shift is held, add to selection; otherwise, select just this NPC
                if (shiftHeld)
                {
                    npcs[i].isSelected = true;
                }
                else
                {
                    // Deselect all others and select only this NPC
                    for (int j = 0; j < npcCount; j++)
                    {
                        if (i != j)
                            npcs[j].isSelected = false;
                    }
                    npcs[i].isSelected = true;
                }
                break;
            }
        }

        // If clicked on empty space, set target position for selected NPCs
        if (!clickedOnNPC)
        {
            for (int i = 0; i < npcCount; i++)
            {
                if (npcs[i].isSelected)
                {
                    npcs[i].targetPosition = mousePosition;
                    // Change state to walking to trigger movement
                    SetNPCState(&npcs[i], NPC_WALKING);
                }
            }
        }
    }
}
