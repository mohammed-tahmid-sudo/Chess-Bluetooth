#include "raylib.h"
#include <stdio.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Chess Board like Chess.com");

    // Board settings
    const int boardSize = 8;
    const int marginLeft = 40;    // Space on the left for rank labels
    const int marginBottom = 40;  // Space at the bottom for file labels
    const int boardPixels = 720;  // Size of the chess board (width and height in pixels)
    const int squareSize = boardPixels / boardSize;  // Size of each square

    // Font sizes for coordinate labels
    int rankFontSize = 20;
    int fileFontSize = 20;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw chess board squares (white's perspective: a1 is bottom-left)
        // Here, row 0 corresponds to rank 1 (bottom) and row 7 to rank 8 (top)
        for (int row = 0; row < boardSize; row++)
        {
            for (int col = 0; col < boardSize; col++)
            {
                int x = marginLeft + col * squareSize;
                int y = marginBottom + row * squareSize;
                // Alternate colors: you can adjust the colors to your liking
                Color squareColor = ((row + col) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                DrawRectangle(x, y, squareSize, squareSize, squareColor);
            }
        }

        // Draw file labels (letters a-h) below the board
        for (int col = 0; col < boardSize; col++)
        {
            char fileLabel[2];
            fileLabel[0] = 'a' + col;
            fileLabel[1] = '\0';

            int textWidth = MeasureText(fileLabel, fileFontSize);
            // Center the letter in the square horizontally
            int x = marginLeft + col * squareSize + (squareSize - textWidth) / 2;
            // Position the letter just below the board with a small gap
            int y = marginBottom - fileFontSize - 5;
            DrawText(fileLabel, x, y, fileFontSize, BLACK);
        }

        // Draw rank labels (numbers 1-8) to the left of the board
        for (int row = 0; row < boardSize; row++)
        {
            char rankLabel[3];
            // Since row 0 is the bottom row, its rank is "1" and row 7 is "8"
            sprintf(rankLabel, "%d", row + 1);

            int textWidth = MeasureText(rankLabel, rankFontSize);
            // Position the number so that it is to the left of the board with a gap
            int x = marginLeft - textWidth - 5;
            // Center the number vertically in the corresponding square
            int y = marginBottom + row * squareSize + (squareSize - rankFontSize) / 2;
            DrawText(rankLabel, x, y, rankFontSize, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

