#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define HEIGHT 800
#define WIDTH 800

struct Color { int r, g, b, a; };
struct Square_Pos { float x, y; };
struct Square { int width, height; };

#define WHITE_COLOR Color{255, 255, 255, 255}
#define BLACK_COLOR Color{0, 0, 0, 255}
#define GRAY_COLOR  Color{128, 128, 128, 255}
#define SQUARE_SIZE 100

vector<string> moveHistory;
bool isWhiteTurn = true;

void setRenderColor(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void drawSquare(SDL_Renderer* renderer, Square_Pos square_pos, Color color) {
    SDL_FRect rect = { square_pos.x, square_pos.y, SQUARE_SIZE, SQUARE_SIZE };
    setRenderColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

void drawBoard(SDL_Renderer* renderer) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isLightSquare = (file + rank) % 2 != 0;
            Color squareColor = isLightSquare ? WHITE_COLOR : GRAY_COLOR;
            Square_Pos square_pos = { file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE };
            drawSquare(renderer, square_pos, squareColor);
        }
    }
}

string getSquareNotation(int file, int rank) {
    return string(1, 'a' + file) + to_string(rank + 1);
}

int board[8][8] = {
    {2, 3, 4, 1, 0, 4, 3, 2},
    {5, 5, 5, 5, 5, 5, 5, 5},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {11, 11, 11, 11, 11, 11, 11, 11},
    {8, 9, 10, 7, 6, 10, 9, 8}
};

SDL_Texture* textures[12] = { nullptr };

void loadTextures(SDL_Renderer* renderer) {
    string pieces[12] = { "WK.bmp", "WQ.bmp", "WR.bmp", "WN.bmp", "WB.bmp", "WP.bmp",
                          "BK.bmp", "BQ.bmp", "BR.bmp", "BN.bmp", "BB.bmp", "BP.bmp" };

    for (int i = 0; i < 12; i++) {
        string filePath = "assets/" + pieces[i];
        SDL_Surface* imageSurface = SDL_LoadBMP(filePath.c_str());
        if (!imageSurface) {
            printf("Failed to load %s: %s\n", pieces[i].c_str(), SDL_GetError());
            continue;
        }
        textures[i] = SDL_CreateTextureFromSurface(renderer, imageSurface);
        SDL_DestroySurface(imageSurface);
    }
}

void renderPieces(SDL_Renderer* renderer) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int piece = board[rank][file];
            if (piece == -1) continue;

            SDL_FRect imageRect = { file * SQUARE_SIZE, (7 - rank) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE };
            SDL_RenderTexture(renderer, textures[piece], NULL, &imageRect);
        }
    }
}

bool dragging = false;
int selectedPiece = -1, selectedX = -1, selectedY = -1;
float draggingX = 0, draggingY = 0;

bool isPathClear(int fromX, int fromY, int toX, int toY) {
    int dx = (toX > fromX) ? 1 : (toX < fromX) ? -1 : 0;
    int dy = (toY > fromY) ? 1 : (toY < fromY) ? -1 : 0;
    int x = fromX + dx;
    int y = fromY + dy;
    while (x != toX || y != toY) {
        if (board[y][x] != -1) return false;
        x += dx;
        y += dy;
    }
    return true;
}

bool pawnMove(int piece, int fromX, int fromY, int toX, int toY, int board[8][8]) {
    int pieceValue = piece;
    bool isWhite = (6 > pieceValue);
    int direction = isWhite ? 1 : -1;


    // Move Forward (Single Step)
    if (fromX == toX && board[toY][toX] == -1) {
        if (toY - fromY == direction) {
            printf("Valid single-step move\n");
            return true;
        }

        // Move Forward (Double Step - Only From Starting Row)
        if ((isWhite && fromY == 1) || (!isWhite && fromY == 6)) {
            if (toY - fromY == 2 * direction && board[fromY + direction][fromX] == -1 && board[toY][toX] == -1) {
                printf("Valid double-step move\n");
                return true;
            }
        }
    }

    // Capture Diagonally
    if (abs(toX - fromX) == 1 && toY - fromY == direction) {
        if (board[toY][toX] != -1) {
            printf("Valid capture move\n");
            return true;
        }
    }

    // En Passant here

    printf("Invalid move\n");
    return false;
}

bool isValidMove(int piece, int fromX, int fromY, int toX, int toY, int board[8][8]) {
    // Ensure target is within board limits
    if (toX < 0 || toX >= 8 || toY < 0 || toY >= 8) return false;
    cout << piece;
    switch (piece) {
    case 0: // King
        return abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1;
    case 6: // black King
        return abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1;
    case 1: // Queen
        return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY))
            && isPathClear(fromX, fromY, toX, toY);
    case 7: // black Queen
        return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY))
            && isPathClear(fromX, fromY, toX, toY);
    case 2: // Rook
        return (fromX == toX || fromY == toY)
            && isPathClear(fromX, fromY, toX, toY);
    case 8: // black Rook
        return (fromX == toX || fromY == toY)
            && isPathClear(fromX, fromY, toX, toY);
    case 3: // Knight
        return (abs(toX - fromX) == 2 && abs(toY - fromY) == 1) ||
            (abs(toX - fromX) == 1 && abs(toY - fromY) == 2);
    case 9: // black Knight
        return (abs(toX - fromX) == 2 && abs(toY - fromY) == 1) ||
            (abs(toX - fromX) == 1 && abs(toY - fromY) == 2);
    case 4: // Bishop
        return abs(fromX - toX) == abs(fromY - toY)
            && isPathClear(fromX, fromY, toX, toY);
    case 10: //black Bishop
        return abs(fromX - toX) == abs(fromY - toY)
            && isPathClear(fromX, fromY, toX, toY);

    case 5: { // Pawn
        return pawnMove(piece, fromX, fromY, toX, toY, board);
    }
    case 11: {// black pawn
        return pawnMove(piece, fromX, fromY, toX, toY, board);
    }
    default:
        return false;
    }
}


void handleMouseClick(int mouseX, int mouseY) {
    int file = mouseX / SQUARE_SIZE;
    int rank = 7 - (mouseY / SQUARE_SIZE);
    int piece = board[rank][file];

    if (piece == -1 || (isWhiteTurn != (piece < 6))) return;  // Prevent selecting the wrong turn's piece
    selectedPiece = piece;
    selectedX = file;
    selectedY = rank;
    draggingX = mouseX - (SQUARE_SIZE / 2);
    draggingY = mouseY - (SQUARE_SIZE / 2);
    dragging = true;
    board[rank][file] = -1;
}


void handleMouseMove(int mouseX, int mouseY) {
    if (dragging) {
        draggingX = mouseX - (SQUARE_SIZE / 2);
        draggingY = mouseY - (SQUARE_SIZE / 2);
    }
}

char getPieceNotation(int piece) {
    if (piece == -1) return '\0'; // No piece
    const string pieceLetters = "KQRNBP";
    return (piece % 6 == 5) ? '\0' : pieceLetters[piece % 6]; // Pawns don't get a letter
}

void recordMove(int fromFile, int fromRank, int toFile, int toRank) {
    if (fromFile == toFile && fromRank == toRank) return; // Ignore invalid moves

    string from = getSquareNotation(fromFile, fromRank);
    string to = getSquareNotation(toFile, toRank);
    bool isCapture = board[toRank][toFile] != -1;

    char pieceLetter = getPieceNotation(selectedPiece);  // Get K, Q, R, N, B (empty for pawns)

    bool needFile = false, needRank = false;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (board[rank][file] == selectedPiece && !(rank == fromRank && file == fromFile)) {
                if (toFile == file) needFile = true;
                if (toRank == rank) needRank = true;
            }
        }
    }

    string moveNotation = "";

    // Add piece letter (not for pawns)
    if (pieceLetter != '\0') moveNotation += pieceLetter;

    // Disambiguation: Add file or rank if needed
    if (needFile) moveNotation += from[0];  // File letter (a-h)
    if (needRank) moveNotation += from[1];  // Rank number (1-8)

    // Capture notation
    if (isCapture) moveNotation += "x";

    // Destination square
    moveNotation += to;

    // Store and print move
    moveHistory.push_back(moveNotation);
    cout << (moveHistory.size() / 2 + 1) << (isWhiteTurn ? ". " : "... ") << moveNotation << endl;

}

void handleMouseRelease(int mouseX, int mouseY) {
    int file = mouseX / SQUARE_SIZE;
    int rank = 7 - (mouseY / SQUARE_SIZE);

    if (dragging) {


        if (selectedX != file || selectedY != rank) {
            int targetPiece = board[rank][file];

            //frnly fire stop
            if (targetPiece != -1 && (selectedPiece < 6) == (targetPiece < 6)) {
                board[selectedY][selectedX] = selectedPiece; // Put it back
            }
            else {
                if (isValidMove(selectedPiece, selectedX, selectedY, file, rank, board)) {
                    recordMove(selectedX, selectedY, file, rank);
                    board[rank][file] = selectedPiece;
                    board[selectedY][selectedX] = -1;
                    isWhiteTurn = !isWhiteTurn;
                }
                else {
                    board[selectedY][selectedX] = selectedPiece;
                }
            }
        }
        else {
            board[selectedY][selectedX] = selectedPiece; // Put it back
        }

        dragging = false;
        selectedPiece = -1;
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL3 Chessboard", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    loadTextures(renderer);
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                handleMouseClick(event.button.x, event.button.y);
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                handleMouseMove(event.motion.x, event.motion.y);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                handleMouseRelease(event.button.x, event.button.y);
            }
        }

        setRenderColor(renderer, BLACK_COLOR);
        SDL_RenderClear(renderer);
        drawBoard(renderer);
        renderPieces(renderer);

        if (dragging && selectedPiece != -1) {
            SDL_FRect imageRect = { draggingX, draggingY, SQUARE_SIZE, SQUARE_SIZE };
            SDL_RenderTexture(renderer, textures[selectedPiece], NULL, &imageRect);
        }
        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 12; i++) {
        if (textures[i]) SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}