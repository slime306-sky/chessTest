#include "PrecomputedMoveData.h"


void PrecomputedMoveData::Init() {
	for (int file = 0; file < 8; ++file) {
		for (int rank = 0; rank < 8; ++rank) {
			int squareIndex = rank * 8 + file;

			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			NumSquaresToEdge[squareIndex][0] = numNorth;
			NumSquaresToEdge[squareIndex][1] = numSouth;
			NumSquaresToEdge[squareIndex][2] = numWest;
			NumSquaresToEdge[squareIndex][3] = numEast;
			NumSquaresToEdge[squareIndex][4] = std::min(numNorth, numWest);
			NumSquaresToEdge[squareIndex][5] = std::min(numSouth, numEast);
			NumSquaresToEdge[squareIndex][6] = std::min(numNorth, numEast);
			NumSquaresToEdge[squareIndex][7] = std::min(numSouth, numWest);

			// Sliding piece moves (rook, bishop, queen)
			for (int dir = 0; dir < 8; ++dir) {
				for (int n = 1; n <= NumSquaresToEdge[squareIndex][dir]; ++n) {
					int targetSquare = squareIndex + DirectionOffsets[dir] * n;

					if (dir <= 3) rookMoves[squareIndex].push_back(targetSquare);    // N, S, W, E
					else bishopMoves[squareIndex].push_back(targetSquare);          // diagonals

					queenMoves[squareIndex].push_back(targetSquare); // All 8 directions
				}
			}

			// King moves (1 square in each direction)
			for (int dir = 0; dir < 8; ++dir) {
				if (NumSquaresToEdge[squareIndex][dir] >= 1) {
					kingMoves[squareIndex].push_back(squareIndex + DirectionOffsets[dir]);
				}
			}

			// Knight moves (hardcoded offsets)
			std::vector<int> knightOffsets = { 17, 15, 10, 6, -17, -15, -10, -6 };
			for (int offset : knightOffsets) {
				int target = squareIndex + offset;

				// Check if within bounds (to prevent wraparound)
				int fileDiff = std::abs((squareIndex % 8) - (target % 8));
				int rankDiff = std::abs((squareIndex / 8) - (target / 8));

				if (target >= 0 && target < 64 && fileDiff <= 2 && rankDiff <= 2) {
					knightMoves[squareIndex].push_back(target);
				}
			}
		}
	}
}
