#pragma once

#include "raylib.h"
#include "asset_manager.h"
#include "npc.h"

// Building states to represent construction progress, completion, and destruction.
typedef enum {
    BUILDING_STATE_CONSTRUCTION,
    BUILDING_STATE_COMPLETED,
    BUILDING_STATE_DESTROYED
} BuildingState;

typedef enum BuildingType {
    BUILDING_CASTLE,
    BUILDING_TOWER,
    BUILDING_HOUSE,
    BUILDING_GRAVEYARD,
    // Add more types here
    BUILDING_TYPE_COUNT // This should always be the last entry
} BuildingType;


typedef struct BuildingConfig {
    const char *typeName;
    const char *constructionSpriteName;
    const char *completedSpriteName;
    const char *destroyedSpriteName;
    float maxHealth;
    const bool animation;
} BuildingConfig;

// Building struct containing position, state, type, and assets needed for display and function.
typedef struct {
    Vector2 position;
    BuildingState state;
    BuildingType type;
    float buildProgress;              // Progress toward completion
    float health;                     // Health of the building
    bool isSelected;                  // Indicates if the building is selected
    const char *unitTypeToSpawn;      // Stores selected unit type for production
    Sprite constructionSprite;        // Sprite displayed during construction
    Sprite completedSprite;           // Sprite displayed upon completion
    Sprite destroyedSprite;           // Sprite displayed when destroyed
    Animation completedAnimation;
    Rectangle collisionBox;
} Building;

// Initializes a building with a specific type, position, and associated sprites.
void InitBuilding(Building *building, Vector2 position, BuildingType type, AssetManager *manager, int faction);

// Updates the building's state based on its progress and manages unit production if applicable.
void UpdateBuilding(Building *building, NPC *npcs, int *npcCount, AssetManager *manager, float deltaTime);

// Produces a unit based on the building's selected unit type.
void ProduceUnit(Building *building, NPC *npcs, int *npcCount, AssetManager *manager);

// Draws the building based on its current state.
void DrawBuilding(Building *building);

// Renders the UI for selecting the unit type to spawn from the building.
void RenderUnitSelectionUI(Building *building);

void HandleBuildingClick(Building *buildings, int buildingCount, NPC *npcs, int npcCount, Vector2 mousePosition, bool mousePressed);