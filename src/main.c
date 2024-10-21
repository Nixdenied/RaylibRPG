#include "raylib.h"
#include "scene_manager.h"
#include <stdio.h> // For file operations
#include <stdlib.h>
#include <time.h>
#include <string.h>

FILE *logFile;

void CustomTraceLog(int logType, const char *text, va_list args)
{
    if (logFile == NULL)
        return;

    time_t now = time(NULL);
    char *timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0'; // Remove the newline character

    fprintf(logFile, "%s [%d] ", timeStr, logType);
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
}

int main(void)
{

    logFile = fopen("game_log.txt", "w");
    if (logFile == NULL)
    {
        CloseWindow();
    }
    SetTraceLogCallback(CustomTraceLog);
    SetConfigFlags(FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_RESIZABLE);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Change My Name");
    SetExitKey(KEY_DELETE);
    InitSceneManager();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        RenderCurrentScene();
        UpdateCurrentScene(deltaTime);
    }

    CloseWindow();
    return 0;
}
