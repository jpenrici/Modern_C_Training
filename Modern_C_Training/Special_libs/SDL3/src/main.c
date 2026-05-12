#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

constexpr float PI = 3.1415926535f;

/**
 * Draws a wireframe sphere using arcs (meridians and parallels).
 */
void DrawSphere(SDL_Renderer* renderer, float centerX, float centerY, float radius, int resolution)
{
    SDL_SetRenderDrawColor(renderer, 57, 255, 20, 255); // green

    // 1. Draw Parallels (Latitude lines)
    for (int i = 1; i < resolution; i++) {
        float phi = PI * (float)i / (float)resolution;
        float currentRadius = radius * sinf(phi);

        for (int j = 0; j < resolution * 2; j++) {
            float theta1 = 2.0f * PI * (float)j / (float)(resolution * 2);
            float theta2 = 2.0f * PI * (float)(j + 1) / (float)(resolution * 2);

            float x1 = centerX + currentRadius * cosf(theta1);
            float y1 = centerY + currentRadius * sinf(theta1);
            float x2 = centerX + currentRadius * cosf(theta2);
            float y2 = centerY + currentRadius * sinf(theta2);

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }

    // 2. Draw Meridians (Longitude lines)
    for (int i = 0; i < resolution * 2; i++) {
        float theta = 2.0f * PI * (float)i / (float)(resolution * 2);

        for (int j = 0; j < resolution; j++) {
            float phi1 = PI * (float)j / (float)resolution;
            float phi2 = PI * (float)(j + 1) / (float)resolution;

            float x1 = centerX + radius * sinf(phi1) * cosf(theta);
            float y1 = centerY + radius * cosf(phi1);
            float x2 = centerX + radius * sinf(phi2) * cosf(theta);
            float y2 = centerY + radius * cosf(phi2);

            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }
    }
}

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    int width = 400;
    int height = 400;

    SDL_Window* window = SDL_CreateWindow("C23 + SDL3 Sphere", width, height, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return 1;
    }

    bool running = true;
    bool needs_redraw = true;
    unsigned int redraw_counter = 0;

    while (running) {
        SDL_Event event;

        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_WINDOW_EXPOSED || event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                needs_redraw = true;
            }
        }

        if (needs_redraw) {
            redraw_counter++;
            SDL_Log("Redraw Triggered! Total count: %u", redraw_counter);

            char title[64];
            snprintf(title, sizeof(title), "C23 + SDL3 Sphere - Redraws: %u", redraw_counter);
            SDL_SetWindowTitle(window, title);

            // Background: Deep Blue
            SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
            SDL_RenderClear(renderer);

            // Draw sphere in the center of the screen

            float centerX = (float)(width / 2.0f);
            float centerY = (float)(height / 2.0f);
            auto radius = fminf(centerX, centerY) * 0.8;
            DrawSphere(renderer, centerX, centerY, radius, 20);

            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
