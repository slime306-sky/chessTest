#pragma once
#include "SDL3/sdl.h"
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <algorithm>
#include <cstdint>        
#include <random>          
#include <unordered_map>

using namespace std;

#define HEIGHT 800
#define WIDTH 800
#define SQUARE_SIZE 100

// Structs
struct Color {
    Uint8 r, g, b, a;
};

struct SquarePos {
    float x, y;
};

struct CastlingRights {
    bool whiteKingside = true;
    bool whiteQueenside = true;
    bool blackKingside = true;
    bool blackQueenside = true;
};

struct Move {
    int startSquare;
    int targetSquare;

    int movedPiece;
    int capturedPiece;
    int promotionPiece;

    enum Type {
        Normal,
        Capture,
        EnPassant,
        Promotion,
        KingsideCastle,
        QueensideCastle
    } type;

    int enPassantCapturedSquare;
    int enPassantSquareBeforeMove;
    bool castlingRightsBeforeMove[4];

    bool whiteKingMovedBefore;
    bool blackKingMovedBefore;
    bool whiteKingsideRookMovedBefore;
    bool whiteQueensideRookMovedBefore;
    bool blackKingsideRookMovedBefore;
    bool blackQueensideRookMovedBefore;

    string san;

    Move(int from, int to, Type t = Normal, int promo = 0);
};


extern array<array<int, 8>, 64> NumSquaresToEdge;
extern array<vector<int>, 64> rookMoves;
extern array<vector<int>, 64> bishopMoves;
extern array<vector<int>, 64> queenMoves;
extern array<vector<int>, 64> kingMoves;
extern array<vector<int>, 64> knightMoves;

// Global constants
extern const Color WHITE_COLOR;
extern const Color BLACK_COLOR;
extern const Color GRAY_COLOR;
extern const Color SUGGESTIONCOLOR;
extern const Color CLICK_COLOR;


extern vector<Move> gameMoves;
