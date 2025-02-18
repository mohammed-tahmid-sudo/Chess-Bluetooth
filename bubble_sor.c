#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For usleep()

#define ARRAY_SIZE 100
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DELAY_TIME 10000 // Time in microseconds (10 milliseconds)

void bubbleSort(int arr[], int n, int* sorted) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap
                int temp = arr[j];
                usleep(DELAY_TIME);
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
            // Mark the step completed in the sorting process
            (*sorted)++;
        }
    }
}

int main(void) {
    // Initialize Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bubble Sort Visualizer");
    SetTargetFPS(60); // Set to 60 frames per second

    // Randomize the array
    srand(time(NULL));
    int arr[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = rand() % SCREEN_HEIGHT;
    }

    int sorted = 0; // To track the sorting steps
    int maxSteps = (ARRAY_SIZE - 1) * (ARRAY_SIZE) / 2; // Total number of steps in bubble sort

    while (!WindowShouldClose()) {
        // Update logic
        if (sorted < maxSteps) {
            bubbleSort(arr, ARRAY_SIZE, &sorted);
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the array bars
        for (int i = 0; i < ARRAY_SIZE; i++) {
            Color barColor = (i == sorted) ? RED : BLUE;
            DrawRectangle(i * (SCREEN_WIDTH / ARRAY_SIZE), SCREEN_HEIGHT - arr[i], SCREEN_WIDTH / ARRAY_SIZE - 1, arr[i], barColor);
        }

        DrawText("Bubble Sort Visualizer", 10, 10, 20, BLACK);
        DrawText(TextFormat("Sorting steps: %d / %d", sorted, maxSteps), 10, 40, 20, BLACK);

        EndDrawing();

        // Add delay to slow down the sorting process
        usleep(DELAY_TIME); // Delay for a brief moment (in microseconds)
    }

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

