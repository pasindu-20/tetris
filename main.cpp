#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

const int screenWidth = 500;
const int screenHeight = 500;
const int boardWidth = 10;
const int boardHeight = 20;
const int blockSize = 25;
const Color blockColors[] = {BLUE, GREEN, RED, YELLOW, PURPLE, ORANGE, MAROON};
const Color bgColor = BLACK;
const Color gridColor = GRAY;
const Color textColor = WHITE;

enum class TetrominoType {
    I, O, T, S, Z, J, L
};

struct Tetromino {
    TetrominoType type;
    std::vector<std::vector<int>> shape;
    Vector2 position;
    Color color;

    Tetromino(TetrominoType t, std::vector<std::vector<int>> s, Vector2 p, Color c)
        : type(t), shape(s), position(p), color(c) {}
};

// Struct to represent each cell on the board
struct Cell {
    bool occupied = false;  // Whether the cell is occupied by a part of a tetromino
    Color color = bgColor;  // The color of the block occupying this cell
};
const std::vector<std::vector<std::vector<int>>> tetrominoShapes = {
    {{0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0},
     {0, 1, 0, 0}},

    {{1, 1},
     {1, 1}},

    {{0, 1, 0},
     {1, 1, 1}},

    {{0, 1, 1},
     {1, 1, 0}},

    {{1, 1, 0},
     {0, 1, 1}},

    {{1, 0, 0},
     {1, 1, 1}},

    {{0, 0, 1},
     {1, 1, 1}}
};

// Create a 2D vector of cells to represent the board
std::vector<std::vector<Cell>> board(boardHeight, std::vector<Cell>(boardWidth));
int score = 0;

std::vector<std::vector<int>> RotateShape(const std::vector<std::vector<int>>& shape) {
    int rows = shape.size();
    int cols = shape[0].size();
    std::vector<std::vector<int>> rotated(cols, std::vector<int>(rows));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            rotated[x][rows - 1 - y] = shape[y][x];
        }
    }

    return rotated;
}
bool CheckCollision(const Tetromino& tetromino) {
    for (int y = 0; y < tetromino.shape.size(); ++y) {
        for (int x = 0; x < tetromino.shape[y].size(); ++x) {
            if (tetromino.shape[y][x]) {
                int boardX = static_cast<int>(tetromino.position.x) + x;
                int boardY = static_cast<int>(tetromino.position.y) + y;
                if (boardX < 0 || boardX >= boardWidth || boardY >= boardHeight || board[boardY][boardX].occupied) {
                    return true;
                }
            }
        }
    }
    return false;
}

void MergeTetromino(Tetromino& tetromino) {
    for (int y = 0; y < tetromino.shape.size(); ++y) {
        for (int x = 0; x < tetromino.shape[y].size(); ++x) {
            if (tetromino.shape[y][x]) {
                int boardX = static_cast<int>(tetromino.position.x) + x;
                int boardY = static_cast<int>(tetromino.position.y) + y;
                board[boardY][boardX].occupied = true;
                board[boardY][boardX].color = tetromino.color;
            }
        }
    }
}
void ClearLines() {
    int linesCleared = 0;
    for (int y = 0; y < boardHeight; ++y) {
        bool fullLine = true;
        for (int x = 0; x < boardWidth; ++x) {
            if (!board[y][x].occupied) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            for (int k = y; k > 0; --k) {
                board[k] = board[k - 1];
            }
            board[0] = std::vector<Cell>(boardWidth);
            linesCleared++;
        }
    }
    score += linesCleared * 100;
}

Tetromino CreateRandomTetromino() {
    TetrominoType type = static_cast<TetrominoType>(rand() % 7);
    std::vector<std::vector<int>> shape = tetrominoShapes[static_cast<int>(type)];
    Vector2 position = { static_cast<float>(boardWidth / 2 - shape[0].size() / 2), 0 };
    Color color = blockColors[static_cast<int>(type)];
    return Tetromino(type, shape, position, color);
}

void DrawBoard() {
    for (int y = 0; y < boardHeight; ++y) {
        for (int x = 0; x < boardWidth; ++x) {
            DrawRectangleLines(x * blockSize, y * blockSize, blockSize, blockSize, gridColor);
            if (board[y][x].occupied) {
                DrawRectangle(x * blockSize, y * blockSize, blockSize, blockSize, board[y][x].color);
            }
        }
    }
}
void DrawTetromino(const Tetromino& tetromino) {
    for (int y = 0; y < tetromino.shape.size(); ++y) {
        for (int x = 0; x < tetromino.shape[y].size(); ++x) {
            if (tetromino.shape[y][x]) {
                int boardX = static_cast<int>(tetromino.position.x) + x;
                int boardY = static_cast<int>(tetromino.position.y) + y;
                DrawRectangle(boardX * blockSize, boardY * blockSize, blockSize, blockSize, tetromino.color);
            }
        }
    }
}

void DrawMenu() {
    DrawRectangle(0, 0, screenWidth, screenHeight, DARKGRAY);
    DrawText("Tetris", screenWidth / 2 - 80, 40, 40, textColor);
    DrawText("Press [ENTER] to Start", screenWidth / 2 - 120, screenHeight / 2 - 20, 20, textColor);
    DrawText("Press [ESC] to Exit", screenWidth / 2 - 90, screenHeight / 2 + 20, 20, textColor);
}

void DrawGameOver() {
    DrawRectangle(0, 0, screenWidth, screenHeight, DARKGRAY);
    DrawText("Game Over", screenWidth / 2 - 70, screenHeight / 2 - 80, 40, textColor);

    std::stringstream scoreText;
    scoreText << "Final Score: " << score;
    DrawText(scoreText.str().c_str(), screenWidth / 2 - 80, screenHeight / 2 - 20, 20, textColor);

    DrawText("Press [ENTER] to Restart", screenWidth / 2 - 120, screenHeight / 2 + 20, 20, textColor);
    DrawText("Press [ESC] to Exit", screenWidth / 2 - 90, screenHeight / 2 + 60, 20, textColor);
}
void DrawScore() {
    std::stringstream scoreText;
    scoreText << "Score: " << score;
    DrawText(scoreText.str().c_str(), screenWidth - 150, 20, 20, textColor);
}

int main() {
    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(60);  // Set FPS to 60

    srand(static_cast<unsigned>(time(0)));

    bool gameStarted = false;
    bool gameOver = false;
    float fallTimer = 0.0f;
    const float fallSpeed = 0.2f;  // Block fall speed in seconds

    Tetromino currentTetromino(static_cast<TetrominoType>(0), {}, {0, 0}, BLUE);

    while (!WindowShouldClose()) {
        if (!gameStarted) {
            BeginDrawing();
            ClearBackground(bgColor);
            DrawMenu();
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                gameStarted = true;
                gameOver = false;
                board = std::vector<std::vector<Cell>>(boardHeight, std::vector<Cell>(boardWidth));
                score = 0;
                currentTetromino = CreateRandomTetromino();
            }

            continue;
        }
        if (gameOver) {
            BeginDrawing();
            ClearBackground(bgColor);
            DrawGameOver();
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                gameStarted = false;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                break;
            }

            continue;
        }

        // Movement and Collision Logic
        if (IsKeyPressed(KEY_LEFT)) {
            currentTetromino.position.x--;
            if (CheckCollision(currentTetromino)) {
                currentTetromino.position.x++;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            currentTetromino.position.x++;
            if (CheckCollision(currentTetromino)) {
                currentTetromino.position.x--;
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            currentTetromino.position.y++;
            if (CheckCollision(currentTetromino)) {
                currentTetromino.position.y--;
            }
        }
        // Move down automatically based on fall speed
        fallTimer += GetFrameTime();
        if (fallTimer >= fallSpeed) {
            fallTimer = 0;
            currentTetromino.position.y++;
            if (CheckCollision(currentTetromino)) {
                currentTetromino.position.y--;
                MergeTetromino(currentTetromino);
                ClearLines();
                currentTetromino = CreateRandomTetromino();
                if (CheckCollision(currentTetromino)) {
                    gameOver = true;
                }
            }
        }

        // Rotate Tetromino
        if (IsKeyPressed(KEY_UP)) {
            Tetromino rotatedTetromino = currentTetromino;
            rotatedTetromino.shape = RotateShape(currentTetromino.shape);
            if (!CheckCollision(rotatedTetromino)) {
                currentTetromino.shape = rotatedTetromino.shape;
            }
        }

        BeginDrawing();
        ClearBackground(bgColor);
        DrawBoard();
        DrawTetromino(currentTetromino);
        DrawScore();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
