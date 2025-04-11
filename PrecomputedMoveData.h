#pragma once

#include "useFullStuff.h"

using namespace std;

class PrecomputedMoveData {
public:
	static constexpr array<int, 8> DirectionOffsets = {
		8, -8, -1, 1, 7, -7, 9, -9
	};

	static array<array<int, 8>, 64> NumSquaresToEdge;
	static array<vector<int>, 64> rookMoves;
	static array<vector<int>, 64> bishopMoves;
	static array<vector<int>, 64> queenMoves;
	static array<vector<int>, 64> kingMoves;
	static array<vector<int>, 64> knightMoves;

	static void Init();
};