#pragma once

#include "Board.h"
#include "useFullStuff.h"
#include "Piece.h"

using namespace std;

class moveGenerator {
public:
    vector<Move> moves;

    vector<Move> GenerateMoves(Board* board);
    vector<Move> GenerateLegalMoves(Board* board);

private:
    void GeneratePawnMoves(int startSquare, int piece, Board* board, vector<Move>& moves);
    void GenerateKnightMoves(int startSquare, int piece, Board* board, vector<Move>& moves);
    void GenerateSlidingMoves(int startSquare, int piece, Board* board, vector<Move>& moves);
    void GenerateKingMoves(int startSquare, int piece, Board* board, vector<Move>& moves);
};