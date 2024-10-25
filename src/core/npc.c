#include "npc.h"
#include "raylib.h"
#include "raymath.h"
#include "asset_manager.h"

extern AssetManager manager;

// Initialize an NPC with a given position, speed, and initial state
void InitNPC(NPC *npc, Vector2 position, float speed, const char *initialAnimationName) {
    npc->position = position;
    npc->speed = speed;
    npc->state = NPC_IDLE;
    npc->isCollidable = true;
    npc->drawName = false;

    // Copy initial animation by value
    Animation initialAnimation = GetAnimation(&manager, initialAnimationName);
    npc->animation = initialAnimation;
    npc->animationIndex = npc->animation.currentFrame;
}

// Update NPC logic based on state and deltaTime
void UpdateNPC(NPC *npc, float deltaTime) {
    switch (npc->state) {
        case NPC_IDLE:
            break;
        case NPC_WALKING:
            npc->position.x += npc->speed * deltaTime;
            UpdateAnimation(npc);
            break;
        case NPC_TALKING:
            break;
        case NPC_ATTACKING:
            break;
        case NPC_DEAD:
            break;
    }
}

// Update the animation for the NPC based on elapsed time
void UpdateAnimation(NPC *npc) {
    npc->animation.elapsedTime += GetFrameTime();

    if (npc->animation.elapsedTime >= npc->animation.frameTime) {
        npc->animation.currentFrame++;
        if (npc->animation.currentFrame >= npc->animation.frameCount) {
            npc->animation.currentFrame = 0;
        }
        npc->animation.elapsedTime = 0;
    }
}

// Draw the NPC on the screen
void DrawNPC(NPC *npc) {
    Rectangle frame = npc->animation.frames[npc->animation.currentFrame];
    DrawTextureRec(npc->animation.texture, frame, npc->position, WHITE);

    if (npc->drawName) {
        DrawText(npc->animation.name, npc->position.x, npc->position.y - 20, 10, RAYWHITE);
    }
}

// Set the state of the NPC and update animation if necessary
void SetNPCState(NPC *npc, NPCState newState) {
    if (npc->state != newState) {
        npc->state = newState;

        // Update animation based on state
        if (npc->state == NPC_WALKING) {
            npc->animation = GetAnimation(&manager, "walking_animation");
        } else if (npc->state == NPC_ATTACKING) {
            npc->animation = GetAnimation(&manager, "attacking_animation");
        }
    }
}
