// Reference: https://www.raylib.com/

#include <raylib.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    const char* windowTitle = "Raylib [Wayland]";
    const char* displayText = "Hello World, Raylib!";
    const int fontSize = 20;

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    int textWidth = MeasureText(displayText, fontSize);
    int posX = (screenWidth / 2) - (textWidth / 2);
    int posY = (screenHeight / 2) - (fontSize / 2);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(displayText, posX, posY, fontSize, RED);
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
