// npc.h

#pragma once

#include "raylib.h"
#include "asset_manager.h"
#include <stdbool.h>

// Enumeration for NPC states
typedef enum
{
    NPC_IDLE,
    NPC_WALKING,
    NPC_TALKING,
    NPC_ATTACKING,
    NPC_DEAD
} NPCState;

typedef struct NPC
{
    Vector2 position;
    Vector2 targetPosition;
    float speed;
    float health; // NPC health
    int strength; // Attack strength
    int defense;  // Defense level
    NPCState state;
    bool isCollidable;
    bool drawName;
    bool isSelected;
        Rectangle boundingBox; // Add this for bounding box
    Animation animation;
    const char *unitType;  // To hold the type, e.g., "Warrior" or "Archer"
    float collisionRadius; // Radius for selection and collision detection
    float separationForce; // Force applied to separate NPCs
} NPC;

// Assuming you have a maximum number of NPCs
#define MAX_NPCS 4000
extern int npcCount;

// Function Prototypes

/**
 * @brief Initializes an NPC with the given position, speed, and initial animation.
 *
 * @param npc Pointer to the NPC to initialize.
 * @param manager Pointer to the AssetManager.
 * @param position Initial position of the NPC.
 * @param speed Movement speed of the NPC (pixels per second).
 * @param initialAnimationName Name of the initial animation to assign.
 */
void InitNPC(NPC *npc, AssetManager *manager, Vector2 position, float speed, const char *initialAnimationName);

/**
 * @brief Updates the NPC's logic based on its current state and the elapsed time.
 *
 * @param npc Pointer to the NPC to update.
 * @param deltaTime Time elapsed since the last frame (in seconds).
 */
void UpdateNPC(NPC *npc, NPC npcs[], float deltaTime);

/**
 * @brief Updates the NPC's current animation based on elapsed time.
 *
 * @param animation Pointer to the Animation to update.
 */
void UpdateAnimation(Animation *animation);

/**
 * @brief Draws the NPC on the screen, including its current animation frame and optional name.
 *
 * @param npc Pointer to the NPC to draw.
 */
void DrawNPC(NPC *npc);

/**
 * @brief Sets the NPC's state and updates its animation accordingly.
 *
 * @param npc Pointer to the NPC.
 * @param newState The new state to assign to the NPC.
 */
void SetNPCState(NPC *npc, NPCState newState);

/**
 * @brief Processes mouse input to handle NPC selection and movement.
 *
 * @param npcs Array of NPCs.
 * @param npcCount Number of NPCs in the array.
 * @param mousePosition Current mouse position.
 * @param mousePressed Boolean indicating if the mouse was pressed.
 */
void HandleNPCMouseInput(NPC *npcs, int npcCount, Vector2 mousePosition, bool mousePressed);