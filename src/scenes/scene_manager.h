#pragma once

#include <stdbool.h>

typedef enum {
    SCENE_MAIN_MENU,
    SCENE_GAMEPLAY,
    SCENE_PAUSE_MENU,
    SCENE_GAME_OVER,
    SCENE_OPTIONS,
    SCENE_DEBUG,
    SCENE_TEST,
    SCENE_RAYLIB_LOGO,
    SCENE_NETWORKING,
    // Add more scenes as needed
} GameScene;

typedef struct {
    GameScene currentScene;
    bool running;
    // Add more global game state variables here if needed
} Game;

void InitSceneManager();
void ChangeScene(GameScene newScene);
void UpdateCurrentScene(float deltaTime);
void RenderCurrentScene();
