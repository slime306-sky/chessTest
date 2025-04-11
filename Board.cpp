#include "Board.h"

Board::Board() {

	Piece::fenToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", Square);
	//Piece::fenToBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R", Square);

	//for (int i = 0; i < 64; i++) {
	//	Square[i] = Piece::None;
	//}
	//
	//Square[0] = Piece::White | Piece::Rook;
	//Square[1] = Piece::White | Piece::Knight;
	//Square[2] = Piece::White | Piece::Bishop;
	//Square[3] = Piece::White | Piece::Queen;
	//Square[4] = Piece::White | Piece::King;
	//Square[5] = Piece::White | Piece::Bishop;
	//Square[6] = Piece::White | Piece::Knight;
	//Square[7] = Piece::White | Piece::Rook;
	//
	//for (int i = 8; i < 16; i++) Square[i] = Piece::White | Piece::Pawn;
	//for (int i = 48; i < 56; i++) Square[i] = Piece::Black | Piece::Pawn;
	//
	//Square[56] = Piece::Black | Piece::Rook;
	//Square[57] = Piece::Black | Piece::Knight;
	//Square[58] = Piece::Black | Piece::Bishop;
	//Square[59] = Piece::Black | Piece::Queen;
	//Square[60] = Piece::Black | Piece::King;
	//Square[61] = Piece::Black | Piece::Bishop;
	//Square[62] = Piece::Black | Piece::Knight;
	//Square[63] = Piece::Black | Piece::Rook;
}

bool Board::isOver() {
	return gameOver;
}

int Board::getCastlingRightsMask() const {
	int mask = 0;
	if (castlingRights.whiteKingside)  mask |= 1 << 0;
	if (castlingRights.whiteQueenside) mask |= 1 << 1;
	if (castlingRights.blackKingside)  mask |= 1 << 2;
	if (castlingRights.blackQueenside) mask |= 1 << 3;
	return mask;
}



void Board::initZobrist() {
	mt19937_64 rng(2025); // fixed seed for reproducibility
	uniform_int_distribution<uint64_t> dist;

	for (int sq = 0; sq < 64; sq++) {
		for (int piece = 0; piece < 12; piece++) {
			zobristTable[sq][piece] = dist(rng);
		}
	}

	zobristBlackToMove = dist(rng);

	for (int i = 0; i < 16; i++) {
		zobristCastlingRights[i] = dist(rng);
	}

	for (int i = 0; i < 8; i++) {
		zobristEnPassant[i] = dist(rng);
	}
}

uint64_t Board::computeZobristHash(const Board& board) const {
	uint64_t hash = 0;

	for (int sq = 0; sq < 64; ++sq) {
		int p = Piece::getPiece(Square[sq]);
		if (p != Piece::None) {
			int pieceIndex = Piece::getIndex(p); // map piece enum to 0-11
			hash ^= zobristTable[sq][pieceIndex];
		}
	}

	if (colorToMove == Piece::Black) {
		hash ^= zobristBlackToMove;
	}

	hash ^= zobristCastlingRights[board.getCastlingRightsMask()];
	if (enPassantSquare != -1) {
		int file = enPassantSquare % 8;
		hash ^= zobristEnPassant[file];
	}

	return hash;
}


void Board::recordPosition(Board& board) {
	uint64_t hash = computeZobristHash(board);
	repetitionMap[hash]++;
}


bool Board::isThreefoldRepetition(Board& board) {
	uint64_t hash = computeZobristHash(board);
	return repetitionMap[hash] >= 3;
}



bool Board::IsPathClear(int from, int to, const Board& board) {
	int diff = to - from;

	// Determine direction
	for (int dir = 0; dir < 8; ++dir) {
		int offset = PrecomputedMoveData::DirectionOffsets[dir];
		for (int n = 1; n <= PrecomputedMoveData::NumSquaresToEdge[from][dir]; ++n) {
			int intermediate = from + offset * n;

			if (intermediate == to)
				return true; // path is clear, nothing in between

			if (board.Square[intermediate] != Piece::None)
				break; // something is blocking the way
		}
	}

	return false; // not a straight-line path or blocked
}

int Board::findKingSquare(int color) const {
	for (int sq = 0; sq < 64; ++sq) {
		if (Piece::IsKing(Square[sq]) && Piece::IsColor(Square[sq], color))
			return sq;
	}
	return -1;
}

bool Board::isSquareAttacked(int square, int byColor, const Board& board) {
	for (int from = 0; from < 64; ++from) {
		int piece = board.Square[from];
		if (!Piece::IsColor(piece, byColor))
			continue;

		int pieceType = Piece::Type(piece);

		switch (pieceType) {
		case Piece::Pawn: {
			int dir = (byColor == Piece::White) ? 1 : -1;
			int targetRank = from / 8 + dir;

			// Diagonal attacks only valid if targetRank is in bounds
			if (targetRank >= 0 && targetRank < 8) {
				int file = from % 8;
				if (file > 0 && from + dir * 8 - 1 == square) return true; // capture left
				if (file < 7 && from + dir * 8 + 1 == square) return true; // capture right
			}
			break;
		}

		case Piece::Knight: {
			for (int move : PrecomputedMoveData::knightMoves[from]) {
				if (move == square) return true;
			}
			break;
		}
		case Piece::Bishop: {
			for (int move : PrecomputedMoveData::bishopMoves[from]) {
				if (move == square) {
					if (Board::IsPathClear(from, move, board)) return true;
				}
			}
			break;
		}
		case Piece::Rook: {
			for (int move : PrecomputedMoveData::rookMoves[from]) {
				if (move == square) {
					if (Board::IsPathClear(from, move, board)) return true;
				}
			}
			break;
		}
		case Piece::Queen: {
			for (int move : PrecomputedMoveData::queenMoves[from]) {
				if (move == square) {
					if (Board::IsPathClear(from, move, board)) return true;
				}
			}
			break;
		}
		case Piece::King: {
			for (int move : PrecomputedMoveData::kingMoves[from]) {
				if (move == square) return true;
			}
			break;
		}
		}
	}

	return false; // No attackers found
}

void Board::makeMove(Move& m, Board& board) {
	int movingPiece = Square[m.startSquare];
	int captured = Square[m.targetSquare];

	// Save moved and captured piece
	m.movedPiece = movingPiece;
	m.capturedPiece = captured;

	// Save board state before move
	m.enPassantSquareBeforeMove = enPassantSquare;

	m.castlingRightsBeforeMove[0] = castlingRights.whiteKingside;
	m.castlingRightsBeforeMove[1] = castlingRights.whiteQueenside;
	m.castlingRightsBeforeMove[2] = castlingRights.blackKingside;
	m.castlingRightsBeforeMove[3] = castlingRights.blackQueenside;

	// Save king/rook movement flags (if you're tracking them)
	m.whiteKingMovedBefore = whiteKingMoved;
	m.blackKingMovedBefore = blackKingMoved;
	m.whiteKingsideRookMovedBefore = whiteKingsideRookMoved;
	m.whiteQueensideRookMovedBefore = whiteQueensideRookMoved;
	m.blackKingsideRookMovedBefore = blackKingsideRookMoved;
	m.blackQueensideRookMovedBefore = blackQueensideRookMoved;

	// Inside makeMove()
	if (m.movedPiece == Piece::Pawn || m.capturedPiece != Piece::None)
		halfmoveClock = 0;
	else
		halfmoveClock++;


	// Handle en passant
	if (m.type == Move::EnPassant) {
		m.enPassantCapturedSquare = (colorToMove == Piece::White) ? m.targetSquare - 8 : m.targetSquare + 8;
		Square[m.enPassantCapturedSquare] = Piece::None;
	}

	// Handle the move
	Square[m.targetSquare] = movingPiece;
	Square[m.startSquare] = Piece::None;

	// Handle promotion
	if (m.type == Move::Promotion && m.promotionPiece != Piece::None) {
		Square[m.targetSquare] = Piece::MakePiece(Piece::GetColor(movingPiece), m.promotionPiece);
	}

	// Handle castling
	if (m.type == Move::KingsideCastle) {
		int rookFrom = (colorToMove == Piece::White) ? 7 : 63;
		int rookTo = (colorToMove == Piece::White) ? 5 : 61;
		Square[rookTo] = Square[rookFrom];
		Square[rookFrom] = Piece::None;
	}
	else if (m.type == Move::QueensideCastle) {
		int rookFrom = (colorToMove == Piece::White) ? 0 : 56;
		int rookTo = (colorToMove == Piece::White) ? 3 : 59;
		Square[rookTo] = Square[rookFrom];
		Square[rookFrom] = Piece::None;
	}

	// Update en passant square
	if (Piece::Type(movingPiece) == Piece::Pawn && abs(m.targetSquare - m.startSquare) == 16) {
		enPassantSquare = (m.startSquare + m.targetSquare) / 2;
	}
	else {
		enPassantSquare = -1;
	}

	// Update castling rights
	if (Piece::Type(movingPiece) == Piece::King) {
		if (colorToMove == Piece::White) {
			whiteKingMoved = true;
			castlingRights.whiteKingside = false;
			castlingRights.whiteQueenside = false;
		}
		else {
			blackKingMoved = true;
			castlingRights.blackKingside = false;
			castlingRights.blackQueenside = false;
		}
	}
	else if (Piece::Type(movingPiece) == Piece::Rook) {
		if (colorToMove == Piece::White) {
			if (m.startSquare == 0) {
				whiteQueensideRookMoved = true;
				castlingRights.whiteQueenside = false;
			}
			if (m.startSquare == 7) {
				whiteKingsideRookMoved = true;
				castlingRights.whiteKingside = false;
			}
		}
		else {
			if (m.startSquare == 56) {
				blackQueensideRookMoved = true;
				castlingRights.blackQueenside = false;
			}
			if (m.startSquare == 63) {
				blackKingsideRookMoved = true;
				castlingRights.blackKingside = false;
			}
		}
	}

	// Switch turn
	colorToMove = Piece::GetOpponentColor(colorToMove);
}

void Board::undoMove(Move& m, Board& board) {
	// Switch turn back
	colorToMove = Piece::GetOpponentColor(colorToMove);

	// Undo promotion (replace promoted piece with pawn)
	if (m.type == Move::Promotion && m.promotionPiece != Piece::None) {
		Square[m.targetSquare] = Piece::MakePiece(Piece::GetColor(m.movedPiece), Piece::Pawn);
	}
	else {
		Square[m.targetSquare] = m.movedPiece;
	}

	// Restore the start square
	Square[m.startSquare] = m.movedPiece;
	if (m.type != Move::EnPassant && m.type != Move::Promotion && m.capturedPiece == Piece::None) {
		Square[m.targetSquare] = Piece::None;
	}

	// Undo en passant capture
	if (m.type == Move::EnPassant) {
		Square[m.targetSquare] = Piece::None;
		Square[m.enPassantCapturedSquare] = m.capturedPiece;
	}
	// Restore captured piece normally
	else if (m.capturedPiece != Piece::None) {
		Square[m.targetSquare] = m.capturedPiece;
	}

	// Undo castling
	if (m.type == Move::KingsideCastle) {
		int rookTo = (colorToMove == Piece::White) ? 5 : 61;
		int rookFrom = (colorToMove == Piece::White) ? 7 : 63;
		Square[rookFrom] = Square[rookTo];
		Square[rookTo] = Piece::None;
	}
	else if (m.type == Move::QueensideCastle) {
		int rookTo = (colorToMove == Piece::White) ? 3 : 59;
		int rookFrom = (colorToMove == Piece::White) ? 0 : 56;
		Square[rookFrom] = Square[rookTo];
		Square[rookTo] = Piece::None;
	}

	// Restore en passant square
	enPassantSquare = m.enPassantSquareBeforeMove;

	// Restore castling rights
	castlingRights.whiteKingside = m.castlingRightsBeforeMove[0];
	castlingRights.whiteQueenside = m.castlingRightsBeforeMove[1];
	castlingRights.blackKingside = m.castlingRightsBeforeMove[2];
	castlingRights.blackQueenside = m.castlingRightsBeforeMove[3];

	// Restore king/rook movement flags
	whiteKingMoved = m.whiteKingMovedBefore;
	blackKingMoved = m.blackKingMovedBefore;
	whiteKingsideRookMoved = m.whiteKingsideRookMovedBefore;
	whiteQueensideRookMoved = m.whiteQueensideRookMovedBefore;
	blackKingsideRookMoved = m.blackKingsideRookMovedBefore;
	blackQueensideRookMoved = m.blackQueensideRookMovedBefore;

	// Remove current position from repetition map
	uint64_t currentHash = computeZobristHash(*this);
	if (repetitionMap.count(currentHash)) {
		repetitionMap[currentHash]--;
		if (repetitionMap[currentHash] == 0)
			repetitionMap.erase(currentHash);
	}
}




char Board::pieceChar(int piece) {
	switch (piece) {
	case Piece::WhitePawn: return 'P';
	case Piece::WhiteKnight: return 'N';
	case Piece::WhiteBishop: return 'B';
	case Piece::WhiteRook: return 'R';
	case Piece::WhiteQueen: return 'Q';
	case Piece::WhiteKing: return 'K';
	case Piece::BlackPawn: return 'p';
	case Piece::BlackKnight: return 'n';
	case Piece::BlackBishop: return 'b';
	case Piece::BlackRook: return 'r';
	case Piece::BlackQueen: return 'q';
	case Piece::BlackKing: return 'k';
	default: return '.';
	}
}


void Board::printBoard(const Board& board) {
	for (int i = 0; i < 64; ++i) {
		std::cout << Board::pieceChar(board.Square[i]) << " ";
		if (i % 8 == 7) std::cout << "\n";
	}
}

bool Board::hasInsufficientMaterial() {
	int whiteBishops = 0, blackBishops = 0;
	int whiteKnights = 0, blackKnights = 0;
	int whiteOther = 0, blackOther = 0;

	for (int sq = 0; sq < 64; ++sq) {
		int p = Piece::getPiece(Square[sq]);
		if (p == Piece::None || p == Piece::WhiteKing || p == Piece::BlackKing)
			continue;

		switch (p) {
		case Piece::WhiteBishop: whiteBishops++; break;
		case Piece::BlackBishop: blackBishops++; break;
		case Piece::WhiteKnight: whiteKnights++; break;
		case Piece::BlackKnight: blackKnights++; break;
		default:
			if (Piece::GetColor(p) == Piece::White)
				whiteOther++;
			else
				blackOther++;
			break;
		}
	}

	// Only kings
	if (whiteBishops == 0 && whiteKnights == 0 && whiteOther == 0 &&
		blackBishops == 0 && blackKnights == 0 && blackOther == 0)
		return true;

	// King + bishop or knight vs king
	if ((whiteBishops == 1 && whiteKnights == 0 && whiteOther == 0 &&
		blackBishops == 0 && blackKnights == 0 && blackOther == 0) ||
		(whiteKnights == 1 && whiteBishops == 0 && whiteOther == 0 &&
			blackBishops == 0 && blackKnights == 0 && blackOther == 0) ||
		(blackBishops == 1 && blackKnights == 0 && blackOther == 0 &&
			whiteBishops == 0 && whiteKnights == 0 && whiteOther == 0) ||
		(blackKnights == 1 && blackBishops == 0 && blackOther == 0 &&
			whiteBishops == 0 && whiteKnights == 0 && whiteOther == 0))
		return true;

	// King + bishop vs king + bishop (same color bishop squares)
	// Optional advanced check if you want to go deeper

	return false;
}



void Board::printMove(const Move& move) {
	std::string typeStr;
	switch (move.type) {
	case Move::Normal: typeStr = "Normal"; break;
	case Move::Capture: typeStr = "Capture"; break;
	case Move::EnPassant: typeStr = "En Passant"; break;
	case Move::Promotion: typeStr = "Promotion"; break;
	case Move::KingsideCastle: typeStr = "Kingside Castle"; break;
	case Move::QueensideCastle: typeStr = "Queenside Castle"; break;
	default: typeStr = "Unknown"; break;
	}

	std::cout << "Move Details:\n";
	std::cout << "  From Square     : " << move.startSquare << "\n";
	std::cout << "  To Square       : " << move.targetSquare << "\n";
	std::cout << "  Type            : " << typeStr << "\n";
	std::cout << "  Moved Piece     : " << move.movedPiece << "\n";
	std::cout << "  Captured Piece  : " << move.capturedPiece << "\n";
	std::cout << "  Promotion Piece : " << move.promotionPiece << "\n";
}


