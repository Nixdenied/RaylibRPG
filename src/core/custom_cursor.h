// custom_cursor.h

#pragma once

#include "raylib.h"
#include "asset_manager.h"
#include "npc.h"
#include "buildings.h"

void InitCustomCursor(AssetManager *manager);
void UpdateCustomCursor(NPC *npcs, int npcCount, Building *buildings, int buildingCount);
void DrawCustomCursor();
void UnloadCustomCursor();
