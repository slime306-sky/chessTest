#include "moveGenerator.h"


using namespace std;
vector<Move> moveGenerator::GenerateMoves(Board* board) {
    moves.clear();

    for (int startSquare = 0; startSquare < 64; ++startSquare) {
        int piece = board->Square[startSquare];

        if (!Piece().IsColor(piece, board->colorToMove))
            continue;

        if (Piece().IsSlidingPiece(piece)) {
            GenerateSlidingMoves(startSquare, piece, board, moves);
        }
        else if (Piece().IsKnight(piece)) {
            GenerateKnightMoves(startSquare, piece, board, moves);
        }
        else if (Piece().IsPawn(piece)) {
            GeneratePawnMoves(startSquare, piece, board, moves);
        }
        else if (Piece().IsKing(piece)) {
            GenerateKingMoves(startSquare, piece, board, moves);
        }
    }
    return moves;
}

vector<Move> moveGenerator::GenerateLegalMoves(Board* board) {
    vector<Move> allMoves = GenerateMoves(board);
    vector<Move> legal;

    for (auto& move : allMoves) {
        Board copy = *board;
        copy.makeMove(move, *board);

        int kingSq = copy.findKingSquare(board->colorToMove);
        if (!copy.isSquareAttacked(kingSq, Piece::GetOpponentColor(board->colorToMove), copy)) {
            legal.push_back(move);
        }
    }

    return legal;
}

void moveGenerator::GeneratePawnMoves(int startSquare, int piece, Board* board, vector<Move>& moves) {
	int direction = Piece::IsWhite(piece) ? 8 : -8;
	int startRank = Piece::IsWhite(piece) ? 1 : 6;
	int promotionRank = Piece::IsWhite(piece) ? 7 : 0;

	int forwardSquare = startSquare + direction;

	// ?? Forward Move
	if (forwardSquare >= 0 && forwardSquare < 64 && board->Square[forwardSquare] == Piece::None) {
		int rank = forwardSquare / 8;
		if (rank == promotionRank) {

			//cout << "simple promotion \n";
			// ?? Promotion options
			moves.push_back(Move(startSquare, forwardSquare, Move::Promotion, Piece::Queen));
			moves.push_back(Move(startSquare, forwardSquare, Move::Promotion, Piece::Rook));
			moves.push_back(Move(startSquare, forwardSquare, Move::Promotion, Piece::Bishop));
			moves.push_back(Move(startSquare, forwardSquare, Move::Promotion, Piece::Knight));
		}
		else {
			moves.push_back({ startSquare, forwardSquare });
		}

		// ?? Double push
		int doublePush = startSquare + 2 * direction;
		if ((startSquare / 8) == startRank && board->Square[doublePush] == Piece::None) {
			moves.push_back({ startSquare, doublePush });
		}
	}

	// ?? En Passant
	int canEnPassant = board->enPassantSquare;
	if (canEnPassant != -1) {
		int leftCap = startSquare + direction - 1;
		int rightCap = startSquare + direction + 1;
		if (leftCap == canEnPassant && (startSquare % 8) != 0)
			moves.push_back({ startSquare, canEnPassant, Move::EnPassant });
		if (rightCap == canEnPassant && (startSquare % 8) != 7)
			moves.push_back({ startSquare, canEnPassant, Move::EnPassant });
	}

	// ?? Captures
	int captures[] = { direction - 1, direction + 1 };
	for (int capOffset : captures) {
		int targetSquare = startSquare + capOffset;
		if (targetSquare < 0 || targetSquare >= 64)
			continue;

		int startFile = startSquare % 8;
		int targetFile = targetSquare % 8;
		if (abs(startFile - targetFile) != 1)
			continue;

		int targetPiece = board->Square[targetSquare];
		if (targetPiece != Piece::None &&
			Piece::IsColor(targetPiece, Piece::GetOpponentColor(board->colorToMove))) {

			int rank = targetSquare / 8;
			if (rank == promotionRank) {
				//cout << "not simmple promotion" << endl;
				// ?? Promotion captures
				moves.push_back(Move(startSquare, targetSquare, Move::Promotion, Piece::Queen));
				moves.push_back(Move(startSquare, targetSquare, Move::Promotion, Piece::Rook));
				moves.push_back(Move(startSquare, targetSquare, Move::Promotion, Piece::Bishop));
				moves.push_back(Move(startSquare, targetSquare, Move::Promotion, Piece::Knight));
			}
			else {
				moves.push_back({ startSquare, targetSquare });
			}
		}
	}
}



void moveGenerator::GenerateKnightMoves(int startSquare, int piece, Board* board, vector<Move>& moves) {
	static const int KnightOffsets[] = { -17, -15, -10, -6, 6, 10, 15, 17 };

	int rank = startSquare / 8;
	int file = startSquare % 8;

	for (int offset : KnightOffsets) {
		int targetSquare = startSquare + offset;
		if (targetSquare < 0 || targetSquare >= 64)
			continue;

		int newRank = targetSquare / 8;
		int newFile = targetSquare % 8;

		if (abs(rank - newRank) + abs(file - newFile) != 3)
			continue; // to prevent wrapping around the board

		int targetPiece = board->Square[targetSquare];

		if (targetPiece == Piece::None || Piece::IsColor(targetPiece, Piece::GetOpponentColor(board->colorToMove))) {
			moves.push_back({ startSquare, targetSquare });
		}
	}
}


void moveGenerator::GenerateSlidingMoves(int startSquare, int piece, Board* board, vector<Move>& moves) {
	vector<int> directions;

	if (Piece().IsRook(piece) || Piece().IsQueen(piece)) {
		int rookDirs[] = { 8, -8, 1, -1 };
		directions.insert(directions.end(), begin(rookDirs), end(rookDirs));
	}
	if (Piece().IsBishop(piece) || Piece().IsQueen(piece)) {
		int bishopDirs[] = { 9, -9, 7, -7 };
		directions.insert(directions.end(), begin(bishopDirs), end(bishopDirs));
	}

	for (int dir : directions) {
		int targetSquare = startSquare;

		while (true) {
			int rank = targetSquare / 8;
			int file = targetSquare % 8;

			targetSquare += dir;

			if (targetSquare < 0 || targetSquare >= 64)
				break;

			int newRank = targetSquare / 8;
			int newFile = targetSquare % 8;

			if (abs(newFile - file) > 1 && (dir == 1 || dir == -1 || dir == 9 || dir == -9 || dir == 7 || dir == -7))
				break;

			int targetPiece = board->Square[targetSquare];

			if (targetPiece == Piece::None) {
				moves.push_back({ startSquare, targetSquare });
			}
			else {
				if (Piece::IsColor(targetPiece, Piece::GetOpponentColor(board->colorToMove))) {
					moves.push_back({ startSquare, targetSquare });
				}
				break; // blocked
			}
		}
	}
}


void moveGenerator::GenerateKingMoves(int startSquare, int piece, Board* board, vector<Move>& moves) {
	static const int KingOffsets[] = {
		-9, -8, -7,
		-1,      1,
		 7,  8,  9
	};

	int rank = startSquare / 8;
	int file = startSquare % 8;

	for (int offset : KingOffsets) {
		int targetSquare = startSquare + offset;

		if (targetSquare < 0 || targetSquare >= 64)
			continue;

		int targetRank = targetSquare / 8;
		int targetFile = targetSquare % 8;

		if (abs(targetFile - file) > 1 || abs(targetRank - rank) > 1)
			continue;

		int targetPiece = board->Square[targetSquare];

		if (targetPiece == Piece::None ||
			Piece::IsColor(targetPiece, Piece::GetOpponentColor(board->colorToMove))) {
			moves.push_back(Move(startSquare, targetSquare));
		}
	}

	// Castling part
	int color = Piece::GetColor(piece);
	bool kingMoved = (color == Piece::White) ? board->whiteKingMoved : board->blackKingMoved;
	bool kingsideRookMoved = (color == Piece::White) ? board->whiteKingsideRookMoved : board->blackKingsideRookMoved;
	bool queensideRookMoved = (color == Piece::White) ? board->whiteQueensideRookMoved : board->blackQueensideRookMoved;

	int homeRank = (color == Piece::White) ? 0 : 7;
	int kingStart = homeRank * 8 + 4;
	int kingsideRook = homeRank * 8 + 7;
	int queensideRook = homeRank * 8 + 0;

	// Kingside
	if (!kingMoved && !kingsideRookMoved &&
		board->Square[kingStart + 1] == Piece::None &&
		board->Square[kingStart + 2] == Piece::None &&
		!board->isSquareAttacked(kingStart, Piece::GetOpponentColor(color), *board) &&
		!board->isSquareAttacked(kingStart + 1, Piece::GetOpponentColor(color), *board) &&
		!board->isSquareAttacked(kingStart + 2, Piece::GetOpponentColor(color), *board)) {

		moves.push_back(Move(kingStart, kingStart + 2, Move::KingsideCastle));
	}

	// Queenside
	if (!kingMoved && !queensideRookMoved &&
		board->Square[kingStart - 1] == Piece::None &&
		board->Square[kingStart - 2] == Piece::None &&
		board->Square[kingStart - 3] == Piece::None &&
		!board->isSquareAttacked(kingStart, Piece::GetOpponentColor(color), *board) &&
		!board->isSquareAttacked(kingStart - 1, Piece::GetOpponentColor(color), *board) &&
		!board->isSquareAttacked(kingStart - 2, Piece::GetOpponentColor(color), *board)) {

		moves.push_back(Move(kingStart, kingStart - 2, Move::QueensideCastle));
	}
}
