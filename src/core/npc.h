// npc.h

#pragma

#include "raylib.h"
#include "asset_manager.h"

// Define NPC states
typedef enum {
    NPC_IDLE,
    NPC_WALKING,
    NPC_TALKING,
    NPC_ATTACKING,
    NPC_DEAD
} NPCState;

// Define NPC structure
typedef struct NPC {
    Vector2 position;
    float speed;
    NPCState state;
    bool isCollidable;
    bool drawName;
    Animation animation; // Changed from Animation* to Animation
} NPC;

// Function Declarations

/**
 * @brief Initializes an NPC with the given parameters.
 *
 * @param npc Pointer to the NPC to initialize.
 * @param position Initial position of the NPC.
 * @param speed Movement speed of the NPC (pixels per second).
 * @param initialAnimationName Name of the initial animation to assign.
 */
void InitNPC(NPC *npc, Vector2 position, float speed, const char *initialAnimationName);

/**
 * @brief Updates the NPC's state and animation based on elapsed time.
 *
 * @param npc Pointer to the NPC to update.
 * @param deltaTime Time elapsed since the last frame (in seconds).
 */
void UpdateNPC(NPC *npc, float deltaTime);

/**
 * @brief Draws the NPC on the screen.
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

void UpdateAnimation(Animation *animation); 