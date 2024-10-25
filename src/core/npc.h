#pragma once

#include "raylib.h"
#include "asset_manager.h"

typedef enum {
    NPC_IDLE,
    NPC_WALKING,
    NPC_TALKING,
    NPC_ATTACKING,
    NPC_DEAD
} NPCState;

typedef struct {
    Vector2 position;
    NPCState state;
    float speed;
    Animation animation; // Directly store the Animation struct here, no pointers
    int animationIndex;  // Index or identifier of the current animation frame
    bool isCollidable;
    bool drawName;
} NPC;

// Function prototypes
void InitNPC(NPC *npc, Vector2 position, float speed, const char *initialAnimationName);
void UpdateNPC(NPC *npc, float deltaTime);
void UpdateAnimation(NPC *npc);
void DrawNPC(NPC *npc);
void SetNPCState(NPC *npc, NPCState newState);
