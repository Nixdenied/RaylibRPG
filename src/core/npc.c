// npc.c

#include "npc.h"
#include "raylib.h"
#include "raymath.h"
#include "asset_manager.h"
#include <stdio.h> // For fprintf

/**
 * @brief Initializes an NPC with the given position, speed, and initial animation.
 *
 * @param npc Pointer to the NPC to initialize.
 * @param position Initial position of the NPC.
 * @param speed Movement speed of the NPC (pixels per second).
 * @param initialAnimationName Name of the initial animation to assign.
 */
void InitNPC(NPC *npc, Vector2 position, float speed, const char *initialAnimationName) {
    npc->position = position;
    npc->speed = speed;
    npc->state = NPC_IDLE;
    npc->isCollidable = true;
    npc->drawName = false;

    // Assign animation by value
    Animation initialAnimation = GetAnimation(&manager, initialAnimationName);
    if (initialAnimation.frameCount > 0) { // Assuming a valid animation has at least one frame
        npc->animation = initialAnimation;
        npc->animation.currentFrame = 0;
        npc->animation.elapsedTime = 0.0f;
    } else {
        fprintf(stderr, "Error: Animation '%s' not found or invalid. NPC initialized without animation.\n", initialAnimationName);
        // Initialize with a default or empty animation if necessary
    }
}

/**
 * @brief Updates the NPC's logic based on its current state and the elapsed time.
 *
 * @param npc Pointer to the NPC to update.
 * @param deltaTime Time elapsed since the last frame (in seconds).
 */
void UpdateNPC(NPC *npc, float deltaTime) {
    switch (npc->state) {
        case NPC_IDLE:
            // Idle behavior (e.g., stand still)
            break;
        case NPC_WALKING:
            // Example: Move to the right
            npc->position.x += npc->speed * deltaTime;
            UpdateAnimation(&npc->animation);
            break;
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
void UpdateAnimation(Animation *animation) {
    if (animation->frameCount <= 0) return; // Safety check

    animation->elapsedTime += GetFrameTime();

    if (animation->elapsedTime >= animation->frameTime) {
        animation->currentFrame++;
        if (animation->currentFrame >= animation->frameCount) {
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
void DrawNPC(NPC *npc) {
    // Draw the current frame of the animation at the NPC's position
    if (npc->animation.frameCount > 0 && npc->animation.texture.id != 0) { // Check if animation is valid
        Rectangle frame = npc->animation.frames[npc->animation.currentFrame];
        DrawTextureRec(npc->animation.texture, frame, npc->position, WHITE);
    }

    // Optionally draw the NPC's name above it
    if (npc->drawName && npc->animation.name != NULL) {
        // Calculate text width and height for centering (optional)
        int textWidth = MeasureText(npc->animation.name, 10);
        Vector2 textPosition = { npc->position.x - textWidth / 2, npc->position.y - 25 };
        DrawText(npc->animation.name, textPosition.x, textPosition.y, 10, RAYWHITE);
    }
}

/**
 * @brief Sets the NPC's state and updates its animation accordingly.
 *
 * @param npc Pointer to the NPC.
 * @param newState The new state to assign to the NPC.
 */
void SetNPCState(NPC *npc, NPCState newState) {
    if (npc->state != newState) {
        npc->state = newState;

        // Update animation based on the new state
        if (npc->state == NPC_WALKING) {
            Animation walkingAnimation = GetAnimation(&manager, "WarriorRed_2");
            if (walkingAnimation.frameCount > 0) { // Check if animation is valid
                npc->animation = walkingAnimation;
                npc->animation.currentFrame = 0;
                npc->animation.elapsedTime = 0.0f;
            } else {
                fprintf(stderr, "Error: Animation 'walking_animation' not found or invalid.\n");
            }
        } else if (npc->state == NPC_ATTACKING) {
            Animation attackingAnimation = GetAnimation(&manager, "WarriorRed_2");
            if (attackingAnimation.frameCount > 0) { // Check if animation is valid
                npc->animation = attackingAnimation;
                npc->animation.currentFrame = 0;
                npc->animation.elapsedTime = 0.0f;
            } else {
                fprintf(stderr, "Error: Animation 'attacking_animation' not found or invalid.\n");
            }
        }
        // Add more states and corresponding animations as needed
    }
}
