#include "npc.h"
#include "raylib.h"
#include "raymath.h"
#include "asset_manager.h"
#include "buildings.h"
#include <stdbool.h>

void InitBuilding(Building *building, Vector2 position, BuildingType type, AssetManager *manager)
{
    building->position = position;
    building->state = BUILDING_STATE_CONSTRUCTION;
    building->type = type;
    building->buildProgress = 0.0f;
    building->health = 100.0f;
    building->isSelected = false;
    building->unitTypeToSpawn = NULL;

    // Assign sprites based on asset names
    building->constructionSprite = GetSprite(manager, "CastleConstruction");
    building->completedSprite = GetSprite(manager, "CastleBlue");
    building->destroyedSprite = GetSprite(manager, "CastleDestroyed");

    // Initialize collision box (centered on the building's position)
    float width = building->completedSprite.texture.width;
    float height = building->completedSprite.texture.height;
    building->collisionBox = (Rectangle){
        position.x - width / 2,
        position.y - height / 2,
        width,
        height};
}

void UpdateBuilding(Building *building, NPC *npcs, int *npcCount, AssetManager *manager, float deltaTime)
{
    switch (building->state)
    {
    case BUILDING_STATE_CONSTRUCTION:
        building->buildProgress += deltaTime * 20.0f;
        if (building->buildProgress >= 100.0f)
        {
            building->state = BUILDING_STATE_COMPLETED;
        }
        break;
    case BUILDING_STATE_COMPLETED:
        if (building->unitTypeToSpawn != NULL)
        { // Condition to produce unit if a type is selected
            ProduceUnit(building, npcs, npcCount, manager);
            building->unitTypeToSpawn = NULL; // Reset after producing
        }
        break;
    case BUILDING_STATE_DESTROYED:
        // Handle destroyed state visuals or logic
        break;
    default:
        break;
    }
}

void ProduceUnit(Building *building, NPC *npcs, int *npcCount, AssetManager *manager)
{
    if (*npcCount >= MAX_NPCS)
    {
        printf("Max NPC limit reached, cannot spawn more units.\n");
        return;
    }

    const char *npcType = building->unitTypeToSpawn ? building->unitTypeToSpawn : "WarriorRed_1";
    // Adjust the spawn offset distance and direction to be directly in front of the building
    float spawnOffset = -150.0f;                                                        // Adjust the offset distance as needed
    Vector2 spawnPosition = Vector2Add(building->position, (Vector2){0, -spawnOffset}); // Place above the building
    InitNPC(&npcs[*npcCount], manager, spawnPosition, 100.0f, npcType);
    npcs[*npcCount].drawName = true;
    npcs[*npcCount].unitType = npcType; // Persist unit type in NPC struct

    printf("NPC Spawned: Type=%s, Health=%.1f, Strength=%d, Defense=%d\n",
           npcs[*npcCount].unitType, npcs[*npcCount].health,
           npcs[*npcCount].strength, npcs[*npcCount].defense);

    (*npcCount)++;
}

void HandleBuildingClick(Building *buildings, int buildingCount, NPC *npcs, int npcCount, Vector2 mousePosition, bool mousePressed)
{
    if (mousePressed)
    {
        bool clickedOnBuilding = false;
        bool clickedOnUI = false;

        // Check for clicks on the building UI first
        for (int i = 0; i < buildingCount; i++)
        {
            if (buildings[i].isSelected)
            {
                Rectangle warriorButton = {100, 80, 120, 30};
                Rectangle archerButton = {100, 120, 120, 30};

                if (CheckCollisionPointRec(mousePosition, warriorButton) || CheckCollisionPointRec(mousePosition, archerButton))
                {
                    clickedOnUI = true;
                    break;
                }
            }
        }

        // If the click is not on the UI, check buildings
        if (!clickedOnUI)
        {
            for (int i = buildingCount - 1; i >= 0; i--)
            {
                Building *building = &buildings[i];

                float width = building->completedSprite.texture.width;
                float height = building->completedSprite.texture.height;
                Rectangle buildingRect = {
                    building->position.x - width / 2,
                    building->position.y - height / 2,
                    width,
                    height};

                if (CheckCollisionPointRec(mousePosition, buildingRect))
                {
                    building->isSelected = true;
                    clickedOnBuilding = true;
                    printf("Building selected!\n");

                    // Deselect all NPCs
                    for (int j = 0; j < npcCount; j++)
                    {
                        npcs[j].isSelected = false;
                    }

                    break; // Stop after selecting the topmost building
                }
                else
                {
                    building->isSelected = false;
                }
            }

            // Deselect buildings only if the click was not on any building or UI
            if (!clickedOnBuilding)
            {
                for (int i = 0; i < buildingCount; i++)
                {
                    buildings[i].isSelected = false;
                }
            }
        }
    }
}

bool ShouldProduceUnit()
{
    return true; // Adjust or replace with actual logic
}

void RenderUnitSelectionUI(Building *building)
{
    if (building->isSelected && building->state == BUILDING_STATE_COMPLETED)
    {
        DrawText("Select Unit to Spawn:", 100, 50, 20, WHITE);

        // Option 1: Warrior
        Rectangle warriorButton = {100, 80, 120, 30};
        DrawRectangleRec(warriorButton, DARKGRAY);
        DrawText("Warrior", 110, 85, 20, WHITE);

        // Check if Warrior option is clicked
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), warriorButton))
        {
            building->unitTypeToSpawn = "WarriorRed_1";
            printf("UI Button Clicked: Warrior selected for spawn.\n"); // Debug message
        }

        // Option 2: Archer
        Rectangle archerButton = {100, 120, 120, 30};
        DrawRectangleRec(archerButton, DARKGRAY);
        DrawText("Archer", 110, 125, 20, WHITE);

        // Check if Archer option is clicked
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), archerButton))
        {
            building->unitTypeToSpawn = "ArcherRed_1";
            printf("UI Button Clicked: Archer selected for spawn.\n"); // Debug message
        }
    }
}

void DrawBuilding(Building *building)
{
    Sprite currentSprite;
    switch (building->state)
    {
    case BUILDING_STATE_CONSTRUCTION:
        currentSprite = building->constructionSprite;
        break;
    case BUILDING_STATE_COMPLETED:
        currentSprite = building->completedSprite;
        break;
    case BUILDING_STATE_DESTROYED:
        currentSprite = building->destroyedSprite;
        break;
    default:
        return;
    }

    // Draw a smaller selection circle if the building is selected
    if (building->isSelected)
    {
        float horizontalRadius = currentSprite.texture.width / 2.0f; // Half the width of the building sprite
        float verticalRadius = currentSprite.texture.height / 3.0f;  // Adjust as needed for the vertical size

        // Draw the ellipse slightly below the building (adjust the Y offset as needed)
        for (int offset = 1; offset <= 3; offset++)
        {
        DrawEllipseLines(building->position.x, building->position.y + currentSprite.texture.height / 5,
                         horizontalRadius + offset, verticalRadius + offset, GREEN);
        }
    }

    // Center the drawing of the sprite texture
    Vector2 drawPosition = {
        building->position.x - currentSprite.texture.width / 2,
        building->position.y - currentSprite.texture.height / 2};

    DrawTexture(currentSprite.texture, drawPosition.x, drawPosition.y, WHITE);
}
