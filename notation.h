#pragma once

#include "useFullStuff.h"
#include "moveGenerator.h"

using namespace std;

class notation {
public:
	static string indexToSquare(int index);
	static char pieceToChar(int piece);
	static bool isCheckAfterMove(const Move& move, const Board& board);
	static bool isMateAfterMove(const Move& move, const Board& board, moveGenerator gen);
	static string moveToSAN(const Move& move, bool isCheck, bool isMate);
	static string toPGN(const vector<Move>& moves, const string& result = "*");
};