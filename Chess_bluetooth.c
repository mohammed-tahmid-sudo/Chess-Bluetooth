// chess_raylib.c
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// === Piece Definitions ===
#define EMPTY 0
// White pieces
#define WP 1
#define WN 2
#define WB 3
#define WR 4
#define WQ 5
#define WK 6
// Black pieces
#define BP 7
#define BN 8
#define BB 9
#define BR 10
#define BQ 11
#define BK 12

// === Board Constants ===
#define SQUARE_SIZE 64   // Each square is 64x64 pixels

// Global board: board[y][x] with y=0 at the top (rank 8) and y=7 at the bottom (rank 1)
int board[8][8];

// === Global Game State Variables ===
int turn = 0; // 0 = White to move, 1 = Black to move
bool gameOver = false;
char gameOverMessage[64] = {0};

// Castling rights:
bool whiteKingsideCastling = true;
bool whiteQueensideCastling = true;
bool blackKingsideCastling = true;
bool blackQueensideCastling = true;

// En passant target square (if any); if none, set to -1
int enPassantX = -1, enPassantY = -1;

// Promotion: when a pawn reaches the last rank, we pause to let the user choose.
bool promotionPending = false;
int promotionX = -1, promotionY = -1;

// === Piece Textures ===
Texture2D pieceTextures[13];  // Index 1..12 used; index 0 unused.

const char* ASSET_PATH = "/home/tahmid/programming_files/Chess_bluetooth/assates/";

// === Function Prototypes ===
bool isWhitePiece(int piece);
bool isBlackPiece(int piece);
bool isSameColor(int piece, int turn);

bool validatePawnMove(int board[8][8], int sx, int sy, int dx, int dy, int piece, int turn);
bool validateKnightMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);
bool validateBishopMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);
bool validateRookMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);
bool validateQueenMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);
bool validateKingMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);

bool isValidMove(int board[8][8], int sx, int sy, int dx, int dy, int turn);

bool isSquareAttacked(int board[8][8], int x, int y, int attackerTurn);
bool isKingInCheck(int board[8][8], int turn);
bool hasLegalMove(int board[8][8], int turn);

void performMove(int sx, int sy, int dx, int dy);
void drawBoard(void);
void drawPieces(void);
void drawPromotionUI(int screenWidth, int screenHeight);
void handlePromotionInput(void);

// === Helper Functions ===
bool isWhitePiece(int piece) {
    return (piece >= WP && piece <= WK);
}

bool isBlackPiece(int piece) {
    return (piece >= BP && piece <= BK);
}

bool isSameColor(int piece, int turn) {
    if(piece == EMPTY) return false;
    return (turn == 0) ? isWhitePiece(piece) : isBlackPiece(piece);
}

// === Piece-Specific Move Validation Functions ===

// Pawn moves – including single/double advance, diagonal capture, and en passant.
bool validatePawnMove(int board[8][8], int sx, int sy, int dx, int dy, int piece, int turn) {
    int direction = (turn == 0) ? -1 : 1;  // White moves up (decreasing y), Black moves down.
    // Single step forward: must be empty.
    if(dx == sx && dy == sy + direction && board[dy][dx] == EMPTY)
        return true;
    // Double step forward from starting rank:
    if(dx == sx && dy == sy + 2 * direction && board[sy + direction][sx] == EMPTY && board[dy][dx] == EMPTY) {
        if(turn == 0 && sy == 6) return true;
        if(turn == 1 && sy == 1) return true;
    }
    // Diagonal capture: if target square is occupied by enemy.
    if((dx == sx + 1 || dx == sx - 1) && dy == sy + direction) {
        if(board[dy][dx] != EMPTY && !isSameColor(board[dy][dx], turn))
            return true;
        // En passant: if destination is empty but equals the en passant target.
        if(board[dy][dx] == EMPTY && dx == enPassantX && dy == enPassantY)
            return true;
    }
    return false;
}

// Knight moves: L-shaped moves.
bool validateKnightMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    int diffx = abs(dx - sx), diffy = abs(dy - sy);
    return ((diffx == 2 && diffy == 1) || (diffx == 1 && diffy == 2));
}

// Bishop moves: Diagonal moves (path must be clear).
bool validateBishopMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    int diffx = dx - sx, diffy = dy - sy;
    if (abs(diffx) != abs(diffy))
        return false;
    int stepX = (diffx > 0) ? 1 : -1;
    int stepY = (diffy > 0) ? 1 : -1;
    for (int i = 1; i < abs(diffx); i++) {
        if (board[sy + i * stepY][sx + i * stepX] != EMPTY)
            return false;
    }
    return true;
}

// Rook moves: Horizontal or vertical (path must be clear).
bool validateRookMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    if(sx != dx && sy != dy)
        return false;
    int stepX = (dx == sx) ? 0 : (dx > sx ? 1 : -1);
    int stepY = (dy == sy) ? 0 : (dy > sy ? 1 : -1);
    int distance = (sx == dx) ? abs(dy - sy) : abs(dx - sx);
    for (int i = 1; i < distance; i++) {
        if (board[sy + i * stepY][sx + i * stepX] != EMPTY)
            return false;
    }
    return true;
}

// Queen moves: Combines rook and bishop.
bool validateQueenMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    if (sx == dx || sy == dy)
        return validateRookMove(board, sx, sy, dx, dy, turn);
    else if (abs(dx - sx) == abs(dy - sy))
        return validateBishopMove(board, sx, sy, dx, dy, turn);
    return false;
}

// King moves: one square in any direction or castling.
bool validateKingMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    int diffx = abs(dx - sx), diffy = abs(dy - sy);
    // Normal one–square move:
    if(diffx <= 1 && diffy <= 1)
        return true;
    // Castling: king moves two squares horizontally with no vertical change.
    if(diffy == 0 && diffx == 2) {
        // For white (king must be on e1, i.e. (4,7)):
        if(turn == 0 && sy == 7 && sx == 4) {
            if(dx == 6 && whiteKingsideCastling) { // kingside
                if(board[7][5] == EMPTY && board[7][6] == EMPTY &&
                   !isSquareAttacked(board, 4,7, 1) &&
                   !isSquareAttacked(board, 5,7, 1) &&
                   !isSquareAttacked(board, 6,7, 1))
                    return true;
            }
            if(dx == 2 && whiteQueensideCastling) { // queenside
                if(board[7][3] == EMPTY && board[7][2] == EMPTY && board[7][1] == EMPTY &&
                   !isSquareAttacked(board, 4,7, 1) &&
                   !isSquareAttacked(board, 3,7, 1) &&
                   !isSquareAttacked(board, 2,7, 1))
                    return true;
            }
        }
        // For black (king on e8, i.e. (4,0)):
        if(turn == 1 && sy == 0 && sx == 4) {
            if(dx == 6 && blackKingsideCastling) {
                if(board[0][5] == EMPTY && board[0][6] == EMPTY &&
                   !isSquareAttacked(board, 4,0, 0) &&
                   !isSquareAttacked(board, 5,0, 0) &&
                   !isSquareAttacked(board, 6,0, 0))
                    return true;
            }
            if(dx == 2 && blackQueensideCastling) {
                if(board[0][3] == EMPTY && board[0][2] == EMPTY && board[0][1] == EMPTY &&
                   !isSquareAttacked(board, 4,0, 0) &&
                   !isSquareAttacked(board, 3,0, 0) &&
                   !isSquareAttacked(board, 2,0, 0))
                    return true;
            }
        }
    }
    return false;
}

// Check whether a move from (sx,sy) to (dx,dy) is valid for the piece at (sx,sy).
// (This does not simulate the move to check for self–check.)
bool isValidMove(int board[8][8], int sx, int sy, int dx, int dy, int turn) {
    // Bounds check
    if(sx < 0 || sx >= 8 || sy < 0 || sy >= 8 ||
       dx < 0 || dx >= 8 || dy < 0 || dy >= 8)
        return false;
    int piece = board[sy][sx];
    if(piece == EMPTY) return false;
    // Must move your own piece.
    if(!isSameColor(piece, turn))
        return false;
    // Cannot capture your own piece.
    if(board[dy][dx] != EMPTY && isSameColor(board[dy][dx], turn))
        return false;
    
    bool valid = false;
    switch(piece) {
        case WP:
        case BP:
            valid = validatePawnMove(board, sx, sy, dx, dy, piece, turn);
            break;
        case WN:
        case BN:
            valid = validateKnightMove(board, sx, sy, dx, dy, turn);
            break;
        case WB:
        case BB:
            valid = validateBishopMove(board, sx, sy, dx, dy, turn);
            break;
        case WR:
        case BR:
            valid = validateRookMove(board, sx, sy, dx, dy, turn);
            break;
        case WQ:
        case BQ:
            valid = validateQueenMove(board, sx, sy, dx, dy, turn);
            break;
        case WK:
        case BK:
            valid = validateKingMove(board, sx, sy, dx, dy, turn);
            break;
        default:
            valid = false;
    }
    return valid;
}

// === Check/Checkmate/Stalemate Functions ===

// Returns true if square (x,y) is attacked by any piece of the side attackerTurn.
bool isSquareAttacked(int board[8][8], int x, int y, int attackerTurn) {
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            int piece = board[j][i];
            if(piece != EMPTY && isSameColor(piece, attackerTurn)) {
                if(isValidMove(board, i, j, x, y, attackerTurn))
                    return true;
            }
        }
    }
    return false;
}

// Finds the king for the given side and returns true if it is in check.
bool isKingInCheck(int board[8][8], int turn) {
    int kingPiece = (turn == 0) ? WK : BK;
    int kingX = -1, kingY = -1;
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            if(board[y][x] == kingPiece) {
                kingX = x;
                kingY = y;
                break;
            }
        }
        if(kingX != -1) break;
    }
    if(kingX == -1) return true; // king missing means check
    int opponent = (turn == 0) ? 1 : 0;
    return isSquareAttacked(board, kingX, kingY, opponent);
}

// Check if the side "turn" has any legal move.
bool hasLegalMove(int board[8][8], int turn) {
    int temp[8][8];
    // For each piece of the side to move...
    for (int sy = 0; sy < 8; sy++){
        for (int sx = 0; sx < 8; sx++){
            int piece = board[sy][sx];
            if(piece != EMPTY && isSameColor(piece, turn)){
                // Try every destination square.
                for (int dy = 0; dy < 8; dy++){
                    for (int dx = 0; dx < 8; dx++){
                        if(isValidMove(board, sx, sy, dx, dy, turn)){
                            // Copy board into temp.
                            for (int y = 0; y < 8; y++){
                                for (int x = 0; x < 8; x++){
                                    temp[y][x] = board[y][x];
                                }
                            }
                            // Simulate the move.
                            int origPiece = temp[sy][sx];
                            temp[dy][dx] = origPiece;
                            temp[sy][sx] = EMPTY;
                            // (Note: We do not simulate castling/en passant state here.)
                            if(!isKingInCheck(temp, turn))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// === Move Execution Function ===

// performMove updates the board and game state (castling rights, en passant, promotion)
// It does not check for move legality – that should be done beforehand.
void performMove(int sx, int sy, int dx, int dy) {
    int piece = board[sy][sx];
    int captured = board[dy][dx]; // captured piece, if any
    bool isCastling = false;
    bool isEnPassant = false;

    // Determine if this is a castling move (king moves two squares horizontally)
    if((piece == WK || piece == BK) && abs(dx - sx) == 2)
        isCastling = true;

    // Determine if this is an en passant capture.
    if((piece == WP || piece == BP) && abs(dx - sx) == 1 && board[dy][dx] == EMPTY) {
        if(dx == enPassantX && dy == enPassantY)
            isEnPassant = true;
    }

    // Move the piece.
    board[dy][dx] = piece;
    board[sy][sx] = EMPTY;

    // Handle en passant capture: remove the captured pawn.
    if(isEnPassant) {
        if(piece == WP)
            board[dy+1][dx] = EMPTY;
        else if(piece == BP)
            board[dy-1][dx] = EMPTY;
    }

    // If a rook is captured from its starting square, update castling rights.
    if(captured == WR) {
        if(dx == 0 && dy == 7) whiteQueensideCastling = false;
        if(dx == 7 && dy == 7) whiteKingsideCastling = false;
    }
    if(captured == BR) {
        if(dx == 0 && dy == 0) blackQueensideCastling = false;
        if(dx == 7 && dy == 0) blackKingsideCastling = false;
    }

    // Handle castling: move the rook as well.
    if(isCastling) {
        if(piece == WK) {
            if(dx == 6) { // kingside
                board[7][5] = board[7][7];
                board[7][7] = EMPTY;
            } else if(dx == 2) { // queenside
                board[7][3] = board[7][0];
                board[7][0] = EMPTY;
            }
        } else if(piece == BK) {
            if(dx == 6) {
                board[0][5] = board[0][7];
                board[0][7] = EMPTY;
            } else if(dx == 2) {
                board[0][3] = board[0][0];
                board[0][0] = EMPTY;
            }
        }
    }

    // If a king moves, lose castling rights.
    if(piece == WK) {
        whiteKingsideCastling = false;
        whiteQueensideCastling = false;
    }
    if(piece == BK) {
        blackKingsideCastling = false;
        blackQueensideCastling = false;
    }
    // If a rook moves, update castling rights.
    if(piece == WR) {
        if(sx == 0 && sy == 7) whiteQueensideCastling = false;
        if(sx == 7 && sy == 7) whiteKingsideCastling = false;
    }
    if(piece == BR) {
        if(sx == 0 && sy == 0) blackQueensideCastling = false;
        if(sx == 7 && sy == 0) blackKingsideCastling = false;
    }

    // Update en passant target:
    if(piece == WP || piece == BP) {
        if(abs(dy - sy) == 2) { // pawn double advance
            enPassantX = sx;
            enPassantY = (sy + dy) / 2;
        } else {
            enPassantX = -1;
            enPassantY = -1;
        }
    } else {
        enPassantX = -1;
        enPassantY = -1;
    }

    // Handle promotion: if a pawn reaches the last rank, flag promotion.
    if(piece == WP && dy == 0) {
        promotionPending = true;
        promotionX = dx;
        promotionY = dy;
    }
    if(piece == BP && dy == 7) {
        promotionPending = true;
        promotionX = dx;
        promotionY = dy;
    }
}

// === Drawing Functions ===

// drawBoard draws the board using alternating colored squares.
void drawBoard(void) {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            Color squareColor = ((x + y) % 2 == 0) ?
                (Color){240, 217, 181, 255} : (Color){181, 136, 99, 255};
            Rectangle rec = { x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
            DrawRectangleRec(rec, squareColor);
        }
    }
}

// drawPieces draws each piece from the board using its corresponding texture.
void drawPieces(void) {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            int piece = board[y][x];
            if(piece != EMPTY) {
                Texture2D tex = pieceTextures[piece];
                // Scale texture to fit SQUARE_SIZE if needed.
                float scale = (float)SQUARE_SIZE / (float)tex.width;
                Vector2 pos = { x * SQUARE_SIZE, y * SQUARE_SIZE };
                DrawTextureEx(tex, pos, 0.0f, scale, WHITE);
            }
        }
    }
}

// Draw promotion UI when a pawn has reached the last rank.
void drawPromotionUI(int screenWidth, int screenHeight) {
    int promoWidth = SQUARE_SIZE * 4;
    int promoHeight = SQUARE_SIZE;
    int startX = (screenWidth - promoWidth) / 2;
    int startY = (screenHeight - promoHeight) / 2;
    DrawRectangle(startX - 10, startY - 10, promoWidth + 20, promoHeight + 20, Fade(RAYWHITE, 0.8f));
    
    // Determine which color is promoting.
    int pawn = board[promotionY][promotionX];
    bool isWhite = (pawn == WP);
    // Promotion options: index 0: Queen, 1: Rook, 2: Bishop, 3: Knight.
    int options[4];
    if(isWhite) {
        options[0] = WQ; options[1] = WR; options[2] = WB; options[3] = WN;
    } else {
        options[0] = BQ; options[1] = BR; options[2] = BB; options[3] = BN;
    }
    for (int i = 0; i < 4; i++){
        Rectangle optionRec = { startX + i * SQUARE_SIZE, startY, SQUARE_SIZE, SQUARE_SIZE };
        DrawRectangleRec(optionRec, LIGHTGRAY);
        // Draw the corresponding piece image.
        Texture2D tex = pieceTextures[ options[i] ];
        float scale = (float)SQUARE_SIZE / (float)tex.width;
        DrawTextureEx(tex, (Vector2){ optionRec.x, optionRec.y }, 0.0f, scale, WHITE);
        DrawRectangleLines(optionRec.x, optionRec.y, optionRec.width, optionRec.height, BLACK);
    }
}

// Handle mouse input for promotion UI.
void handlePromotionInput(void) {
    if(!promotionPending) return;
    int screenWidth = 8 * SQUARE_SIZE;
    int screenHeight = 8 * SQUARE_SIZE;
    int promoWidth = SQUARE_SIZE * 4;
    int promoHeight = SQUARE_SIZE;
    int startX = (screenWidth - promoWidth) / 2;
    int startY = (screenHeight - promoHeight) / 2;
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        // Check if click falls within one of the 4 option boxes.
        if(mousePos.x >= startX && mousePos.x <= startX + promoWidth &&
           mousePos.y >= startY && mousePos.y <= startY + promoHeight) {
            int option = (int)((mousePos.x - startX) / SQUARE_SIZE);
            // Determine new piece for promotion.
            int pawn = board[promotionY][promotionX];
            bool isWhite = (pawn == WP);
            int newPiece = 0;
            switch(option) {
                case 0: newPiece = (isWhite) ? WQ : BQ; break;
                case 1: newPiece = (isWhite) ? WR : BR; break;
                case 2: newPiece = (isWhite) ? WB : BB; break;
                case 3: newPiece = (isWhite) ? WN : BN; break;
                default: newPiece = (isWhite) ? WQ : BQ;
            }
            board[promotionY][promotionX] = newPiece;
            promotionPending = false;
            // After promotion, switch turn.
            turn = (turn == 0) ? 1 : 0;
            // After a promotion, check for game end.
            if(!hasLegalMove(board, turn)) {
                if(isKingInCheck(board, turn))
                    sprintf(gameOverMessage, "Checkmate! %s wins!", (turn == 0) ? "Black" : "White");
                else
                    sprintf(gameOverMessage, "Stalemate!");
                gameOver = true;
            }
        }
    }
}

// === Main Program ===
int main(void) {
    const int screenWidth = 8 * SQUARE_SIZE;
    const int screenHeight = 8 * SQUARE_SIZE;
    
    InitWindow(screenWidth, screenHeight, "Chess Game - raylib (Perfect Rules with Piece Images)");
    SetTargetFPS(60);
    
    // --- Load Piece Textures ---
    char fileName[256];
    // White pieces.
    sprintf(fileName, "%sw_pawn.png", ASSET_PATH);
    pieceTextures[WP] = LoadTexture(fileName);
    
    sprintf(fileName, "%sw_knight.png", ASSET_PATH);
    pieceTextures[WN] = LoadTexture(fileName);
    
    sprintf(fileName, "%sw_bishop.png", ASSET_PATH);
    pieceTextures[WB] = LoadTexture(fileName);
    
    sprintf(fileName, "%sw-rook.png", ASSET_PATH);  // note the dash
    pieceTextures[WR] = LoadTexture(fileName);
    
    sprintf(fileName, "%sw_queen.png", ASSET_PATH);
    pieceTextures[WQ] = LoadTexture(fileName);
    
    sprintf(fileName, "%sw_king.png", ASSET_PATH);
    pieceTextures[WK] = LoadTexture(fileName);
    
    // Black pieces.
    sprintf(fileName, "%sb_pawn.png", ASSET_PATH);
    pieceTextures[BP] = LoadTexture(fileName);
    
    sprintf(fileName, "%sb_knight.png", ASSET_PATH);
    pieceTextures[BN] = LoadTexture(fileName);
    
    sprintf(fileName, "%sb_bishop.png", ASSET_PATH);
    pieceTextures[BB] = LoadTexture(fileName);
    
    sprintf(fileName, "%sb_rook.png", ASSET_PATH);
    pieceTextures[BR] = LoadTexture(fileName);
    
    sprintf(fileName, "%sb_queen.png", ASSET_PATH);
    pieceTextures[BQ] = LoadTexture(fileName);
    
    sprintf(fileName, "%sb_king.png", ASSET_PATH);
    pieceTextures[BK] = LoadTexture(fileName);
    
    // --- Initialize Board with Standard Starting Position ---
    // Black pieces (top rows)
    board[0][0] = BR; board[0][1] = BN; board[0][2] = BB; board[0][3] = BQ;
    board[0][4] = BK; board[0][5] = BB; board[0][6] = BN; board[0][7] = BR;
    for (int x = 0; x < 8; x++) board[1][x] = BP;
    // Empty rows
    for (int y = 2; y < 6; y++){
        for (int x = 0; x < 8; x++){
            board[y][x] = EMPTY;
        }
    }
    // White pieces (bottom rows)
    for (int x = 0; x < 8; x++) board[6][x] = WP;
    board[7][0] = WR; board[7][1] = WN; board[7][2] = WB; board[7][3] = WQ;
    board[7][4] = WK; board[7][5] = WB; board[7][6] = WN; board[7][7] = WR;
    
    // Variables for move selection.
    int selectedX = -1, selectedY = -1;
    
    // --- Main Game Loop ---
    while (!WindowShouldClose()) {
        // --- Update ---
        if (!gameOver && !promotionPending) {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                int x = mousePos.x / SQUARE_SIZE;
                int y = mousePos.y / SQUARE_SIZE;
                if(x >= 0 && x < 8 && y >= 0 && y < 8) {
                    // If no piece is selected, try to select one.
                    if(selectedX == -1 && selectedY == -1) {
                        int piece = board[y][x];
                        if(piece != EMPTY && isSameColor(piece, turn))
                            { selectedX = x; selectedY = y; }
                    } else {
                        // Attempt to move the selected piece.
                        if(isValidMove(board, selectedX, selectedY, x, y, turn)) {
                            // Make a copy of the board state to test for self–check.
                            int temp[8][8];
                            for (int yy = 0; yy < 8; yy++){
                                for (int xx = 0; xx < 8; xx++){
                                    temp[yy][xx] = board[yy][xx];
                                }
                            }
                            performMove(selectedX, selectedY, x, y);
                            // If our king is now in check, the move is illegal.
                            if(isKingInCheck(board, turn)) {
                                // Restore board from temp.
                                for (int yy = 0; yy < 8; yy++){
                                    for (int xx = 0; xx < 8; xx++){
                                        board[yy][xx] = temp[yy][xx];
                                    }
                                }
                            } else {
                                // Only switch turn if no promotion is pending.
                                if(!promotionPending)
                                    turn = (turn == 0) ? 1 : 0;
                                // Check for game end.
                                if(!hasLegalMove(board, turn)) {
                                    if(isKingInCheck(board, turn))
                                        sprintf(gameOverMessage, "Checkmate! %s wins!", (turn == 0) ? "Black" : "White");
                                    else
                                        sprintf(gameOverMessage, "Stalemate!");
                                    gameOver = true;
                                }
                            }
                        }
                        selectedX = -1;
                        selectedY = -1;
                    }
                }
            }
        }
        
        // If a pawn is awaiting promotion, handle promotion UI input.
        if(promotionPending)
            handlePromotionInput();
        
        // --- Draw ---
        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawBoard();
            drawPieces();
            // Highlight selected square.
            if(selectedX != -1 && selectedY != -1) {
                DrawRectangleLines(selectedX * SQUARE_SIZE, selectedY * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);
            }
            // If promotion is pending, draw the promotion UI.
            if(promotionPending)
                drawPromotionUI(screenWidth, screenHeight);
            // If game is over, display the game–over message.
            if(gameOver)
                DrawText(gameOverMessage, 10, screenHeight - 30, 20, RED);
        EndDrawing();
    }
    
    // --- Unload Piece Textures ---
    for (int i = 1; i < 13; i++) {
        UnloadTexture(pieceTextures[i]);
    }
    
    CloseWindow();
    return 0;
}

