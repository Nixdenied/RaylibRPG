#include "raylib.h"
#include "main_menu_scene.h"
#include "scene_manager.h"
#include "debug_scene.h"

// Menu options
typedef enum
{
    MENU_START,
    MENU_OPTIONS,
    MENU_MULTIPLAYER,
    MENU_EXIT,
    MENU_COUNT // Number of menu items
} MenuOption;

// Current selected menu option
static MenuOption selectedOption = MENU_START;

// Button properties
static Rectangle buttonRects[MENU_COUNT];
static const int buttonWidth = 350;
static const int buttonHeight = 60;
static const int buttonRadius = 10; // Button corner radius
static Font font;

// Colors
static Color normalColor = DARKGRAY;
static Color hoverColor = GREEN;
static Color selectedColor = DARKGREEN;

// Title properties
static const int titleFontSize = 100;

void InitMainMenuScene()
{
    font = LoadFont("/opt/git/raylibGame/bin/Kenney_Future_Square.ttf");
    // Calculate the center of the screen for button positioning
    int screenWidth = GetRenderWidth();
    int screenHeight = GetRenderHeight();
    int startY = (screenHeight - (buttonHeight * MENU_COUNT + 20 * (MENU_COUNT - 1))) / 2;

    // Initialize button rectangles
    for (int i = 0; i < MENU_COUNT; i++)
    {
        buttonRects[i] = (Rectangle){
            (screenWidth - buttonWidth) / 2,
            startY + i * (buttonHeight + 20),
            buttonWidth,
            buttonHeight};
    }
}

void UpdateMainMenuScene(float deltaTime)
{
    Vector2 mousePosition = GetMousePosition();

    // Handle mouse input for button hover and selection
    for (int i = 0; i < MENU_COUNT; i++)
    {
        if (CheckCollisionPointRec(mousePosition, buttonRects[i]))
        {
            selectedOption = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                switch (selectedOption)
                {
                case MENU_START:
                    ChangeScene(SCENE_TEST);
                    break;
                case MENU_OPTIONS:
                    ChangeScene(SCENE_TILE_PLACEMENT);
                    break;
                case MENU_MULTIPLAYER:
                    ChangeScene(SCENE_DEBUG);
                    break;
                case MENU_EXIT:
                    CloseWindow(); // Close the window to exit
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Handle keyboard input for navigation and selection
    if (IsKeyPressed(KEY_DOWN))
    {
        selectedOption = (selectedOption + 1) % MENU_COUNT;
    }
    if (IsKeyPressed(KEY_UP))
    {
        selectedOption = (selectedOption - 1 + MENU_COUNT) % MENU_COUNT;
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        switch (selectedOption)
        {
        case MENU_START:
            ChangeScene(SCENE_DEBUG);
            break;
        case MENU_OPTIONS:
            ChangeScene(SCENE_TILE_PLACEMENT);
            break;
        case MENU_MULTIPLAYER:
            ChangeScene(SCENE_NETWORKING);
            break;
        case MENU_EXIT:
            CloseWindow(); // Close the window to exit
            break;
        default:
            break;
        }
    }
}

void RenderMainMenuScene()
{
    ClearBackground(RAYWHITE);

    // Draw game title
    int titleWidth = MeasureTextEx(font, "RPG", titleFontSize, 0).x;
    DrawTextEx(font, "RPG",
               (Vector2){(GetRenderWidth() - titleWidth) / 2.0f, 100.0f},
               titleFontSize, 0, GREEN);

    // Draw buttons with rounded corners
    const char *menuOptions[MENU_COUNT] = {"Test Map", "Create Map", "Debug Asset Manager", "Exit"};
    for (int i = 0; i < MENU_COUNT; i++)
    {
        Color buttonColor = normalColor;
        if (selectedOption == i)
        {
            buttonColor = hoverColor;
        }
        if (CheckCollisionPointRec(GetMousePosition(), buttonRects[i]))
        {
            buttonColor = selectedColor;
        }

        // Draw the rounded rectangle button
        DrawRectangleRounded(buttonRects[i], 0.2f, buttonRadius, buttonColor);

        // Draw the text centered within the button
        Vector2 textSize = MeasureTextEx(font, menuOptions[i], 30, 0);
        DrawTextEx(font, menuOptions[i],
                   (Vector2){
                       buttonRects[i].x + (buttonWidth - textSize.x) / 2.0f,
                       buttonRects[i].y + (buttonHeight - textSize.y) / 2.0f},
                   30, 0, RAYWHITE);
    }
}

void UnloadMainMenuScene()
{
    // No resources to unload for the main menu
}
