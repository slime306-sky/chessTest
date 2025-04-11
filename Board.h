// Board.h
#pragma once

#include "Piece.h"
#include "useFullStuff.h"
#include "PrecomputedMoveData.h"


class Board {
public:


    int Square[64];
    int colorToMove = Piece::White;

    int enPassantSquare = -1;
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteKingsideRookMoved = false;
    bool whiteQueensideRookMoved = false;
    bool blackKingsideRookMoved = false;
    bool blackQueensideRookMoved = false;
    uint64_t zobristTable[64][12];
    uint64_t zobristBlackToMove;
    uint64_t zobristCastlingRights[16];
    uint64_t zobristEnPassant[8];
    std::unordered_map<uint64_t, int> repetitionMap;
    int halfmoveClock = 0;
    bool gameOver = false;

    CastlingRights castlingRights;

    Board();

    bool isOver();
    int getCastlingRightsMask() const;
    void initZobrist();
    uint64_t computeZobristHash(const Board& board) const;
    void recordPosition(Board& board);
    bool isThreefoldRepetition(Board& board);
    static bool IsPathClear(int from, int to, const Board& board);
    int findKingSquare(int color) const;
    bool isSquareAttacked(int square, int byColor, const Board& board);
    void makeMove(Move& m, Board& board);
    void undoMove(Move& m, Board& board);
    char pieceChar(int piece);
    void printBoard(const Board& board);
    bool hasInsufficientMaterial();
    void printMove(const Move& move);
};