#include "raylib_wrapper.h"
#include "scene_manager.h"
#include "debug_scene.h"

static GameScene currentScene;

void InitSceneManager()
{
    currentScene = SCENE_DEBUG;
    InitDebugScene();
}

void ChangeScene(GameScene newScene)
{
    currentScene = newScene;

    switch (currentScene)
    {
    case SCENE_DEBUG:
        InitDebugScene();
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
    }
    EndDrawing();
}
