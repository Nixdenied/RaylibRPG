#include "resources.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

void InitResources(Resources *resources) {
    if (resources != NULL) {
        resources->wood = 0; // Starting amount of wood
        resources->gold = 0; // Starting amount of gold
    }
}

void DrawResources(const Resources *resources, int screenWidth) {
    if (resources != NULL) {
        // Define text color and font size
        int fontSize = 20;
        Color textColor = WHITE;
        int padding = 10;

        // Create resource strings
        char woodText[50];
        char goldText[50];
        snprintf(woodText, sizeof(woodText), "Wood: %d", resources->wood);
        snprintf(goldText, sizeof(goldText), "Gold: %d", resources->gold);

        // Measure text widths
        int woodTextWidth = MeasureText(woodText, fontSize);
        int goldTextWidth = MeasureText(goldText, fontSize);

        // Calculate positions
        int xPos = screenWidth - padding - (woodTextWidth > goldTextWidth ? woodTextWidth : goldTextWidth);
        int woodYPos = padding;
        int goldYPos = woodYPos + fontSize + 5; // 5px gap between lines

        // Draw the text
        DrawText(woodText, xPos, woodYPos, fontSize, textColor);
        DrawText(goldText, xPos, goldYPos, fontSize, textColor);
    }
}

void AddWood(Resources *resources, int amount) {
    if (resources != NULL) {
        resources->wood += amount;
        printf("Added %d wood. Total wood: %d\n", amount, resources->wood);
    }
}

void AddGold(Resources *resources, int amount) {
    if (resources != NULL) {
        resources->gold += amount;
        printf("Added %d gold. Total gold: %d\n", amount, resources->gold);
    }
}
