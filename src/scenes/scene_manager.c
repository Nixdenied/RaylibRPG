#include "raylib_wrapper.h"
#include "scene_manager.h"
#include "debug_scene.h"
#include "tile_placement_scene.h" // Include the tile placement scene header

static GameScene currentScene;

void InitSceneManager()
{
    currentScene = SCENE_TILE_PLACEMENT; // Initialize to the debug scene
    InitTilePlacementScene();
}

void ChangeScene(GameScene newScene)
{
    currentScene = newScene;

    switch (currentScene)
    {
    case SCENE_DEBUG:
        InitDebugScene();
        break;
    case SCENE_TILE_PLACEMENT:
        InitTilePlacementScene(); // Initialize tile placement scene
        break;
    }
}

void UpdateCurrentScene(float deltaTime)
{
    switch (currentScene)
    {
    case SCENE_DEBUG:
        UpdateDebugScene(deltaTime);
        break;
    case SCENE_TILE_PLACEMENT:
        UpdateTilePlacementScene(deltaTime); // Update tile placement scene
        break;
    }
}

void RenderCurrentScene()
{
    BeginDrawing();
    switch (currentScene)
    {
    case SCENE_DEBUG:
        RenderDebugScene();
        break;
    case SCENE_TILE_PLACEMENT:
        RenderTilePlacementScene(); // Render tile placement scene
        break;
    }
    EndDrawing();
}
