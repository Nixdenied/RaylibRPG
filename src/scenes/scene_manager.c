#include "raylib_wrapper.h"
#include "scene_manager.h"
#include "debug_scene.h"
#include "tile_placement_scene.h"
#include "test_map_scene.h"
#include "main_menu_scene.h"

static GameScene currentScene;

void InitSceneManager()
{
    currentScene = SCENE_MAIN_MENU; // Initialize to the debug scene
    InitMainMenuScene();
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
    case SCENE_TEST:
        InitTestMapScene();
        break;
    case SCENE_MAIN_MENU:
        InitMainMenuScene();
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
    case SCENE_TEST:
        UpdateTestMapScene(deltaTime);
        break;
    case SCENE_MAIN_MENU:
        UpdateMainMenuScene(deltaTime);
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
    case SCENE_TEST:
        RenderTestMapScene();
        break;
    case SCENE_MAIN_MENU:
        RenderMainMenuScene();
        break;
    }
    EndDrawing();
}
