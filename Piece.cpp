#include "Piece.h"
#include "useFullStuff.h"

using namespace std;

bool Piece::IsColor(int piece, int color) {
	return (piece & 8) == color;
}

bool Piece::IsSlidingPiece(int piece) {
	int type = piece & 0b111;
	return (type == Piece::Bishop || type == Piece::Rook || type == Piece::Queen);
}
int Piece::getIndex(int piece) {
	// White pieces: 1–6 → index 0–5
	// Black pieces: 9–14 → index 6–11
	if (piece >= 1 && piece <= 6) {
		return piece - 1;
	}
	else if (piece >= 9 && piece <= 14) {
		return piece - 3; // 9 → 6, 14 → 11
	}
	return -1; // None or invalid piece
}

int Piece::getPiece(int piece) {
	if (piece == Piece::None)
		return Piece::None; // skip or treat as no piece

	return piece & 0b111; // mask to get only the piece type (lower 3 bits)
}

bool Piece::MovesInDirection(int piece, int dir) {
	int type = piece & 0b111;
	if (type == Piece::Rook) return dir < 4;
	if (type == Piece::Bishop) return dir >= 4;
	if (type == Piece::Queen) return dir < 8;
	return false;
}
int Piece::MakePiece(int color, int type) {
	return color | type;
}
int Piece::GetOpponentColor(int color) {
	return color == Piece::White ? Piece::Black : Piece::White;
}
int Piece::GetColor(int piece) {
	if (IsWhite(piece)) return Piece::White;
	if (IsBlack(piece)) return Piece::Black;
	return -1; // Invalid or empty square
}
int Piece::oppositeColor(int color) {
	return (color == Piece::White) ? Piece::Black : Piece::White;
}
bool Piece::IsKnight(int piece) {
	return piece == Piece::WhiteKnight || piece == Piece::BlackKnight;
}
bool Piece::IsPawn(int piece) {
	return piece == Piece::WhitePawn || piece == Piece::BlackPawn;
}
bool Piece::IsKing(int piece) {
	return piece == Piece::WhiteKing || piece == Piece::BlackKing;
}
bool Piece::IsRook(int piece) {
	return piece == Piece::WhiteRook || piece == Piece::BlackRook;
}
bool Piece::IsBishop(int piece) {
	return piece == Piece::WhiteBishop || piece == Piece::BlackBishop;
}
bool Piece::IsQueen(int piece) {
	return piece == Piece::WhiteQueen || piece == Piece::BlackQueen;
}
bool Piece::IsWhite(int piece) {
	return piece >= Piece::WhitePawn && piece <= Piece::WhiteKing;
}
bool Piece::IsBlack(int piece) {
	return piece >= Piece::BlackPawn && piece <= Piece::BlackKing;
}

int Piece::Type(int piece) {
	if (piece >= 1 && piece <= 6) return piece;       // White pieces
	else if (piece >= 9 && piece <= 14) return piece - 8; // Black pieces
	else return Piece::None;
}

void Piece::fenToBoard(const string& fen1, int Square[64]) {
	string fen = fen1;

	int rank = 7; // Start from rank 8 (top)
	int file = 0;

	for (char c : fen) {
		if (c == ' ') break; // Stop parsing at the end of the piece placement part

		if (c == '/') {
			rank--; // Move down one rank
			file = 0;
			continue;
		}
		else if (isdigit(c)) {
			file += c - '0'; // Skip empty squares
		}
		else {
			int piece = Piece::None;

			switch (tolower(c)) {
			case 'p': piece = Piece::Pawn;   break;
			case 'n': piece = Piece::Knight; break;
			case 'b': piece = Piece::Bishop; break;
			case 'r': piece = Piece::Rook;   break;
			case 'q': piece = Piece::Queen;  break;
			case 'k': piece = Piece::King;   break;
			}

			if (islower(c)) piece |= Piece::Black;
			else            piece |= Piece::White;

			int index = rank * 8 + file;
			Square[index] = piece;
			file++;
		}
	}

	// Optional: fill rest of board with Piece::None
	for (int i = 0; i < 64; ++i) {
		if (Square[i] == 0)
			Square[i] = Piece::None;
	}
}