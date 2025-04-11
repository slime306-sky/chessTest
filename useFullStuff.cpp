
#include "useFullStuff.h"
#include "PrecomputedMoveData.h"

// Move constructor
Move::Move(int from, int to, Type t, int promo)
    : startSquare(from), targetSquare(to), type(t), promotionPiece(promo),
    movedPiece(0), capturedPiece(0),
    enPassantCapturedSquare(-1), enPassantSquareBeforeMove(-1),
    whiteKingMovedBefore(false), blackKingMovedBefore(false),
    whiteKingsideRookMovedBefore(false), whiteQueensideRookMovedBefore(false),
    blackKingsideRookMovedBefore(false), blackQueensideRookMovedBefore(false) {
    for (int i = 0; i < 4; ++i) castlingRightsBeforeMove[i] = false;
}

// Global colors
const Color WHITE_COLOR = { 255, 255, 255, 255 };
const Color BLACK_COLOR = { 0, 0, 0, 255 };
const Color GRAY_COLOR = { 128, 128, 128, 255 };
const Color SUGGESTIONCOLOR = { 100, 100, 0, 255 };
const Color CLICK_COLOR = { 0, 255, 0, 100 };

// Global move list
vector<Move> gameMoves;

// Precomputed move data
array<array<int, 8>, 64> PrecomputedMoveData::NumSquaresToEdge = {};
array<vector<int>, 64> PrecomputedMoveData::rookMoves = {};
array<vector<int>, 64> PrecomputedMoveData::bishopMoves = {};
array<vector<int>, 64> PrecomputedMoveData::queenMoves = {};
array<vector<int>, 64> PrecomputedMoveData::kingMoves = {};
array<vector<int>, 64> PrecomputedMoveData::knightMoves = {};