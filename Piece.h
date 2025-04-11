#pragma once

#include "useFullStuff.h";

using namespace std;

class Piece {
public:
	static const int None = 0;
	static const int Pawn = 1;
	static const int Knight = 2;
	static const int Bishop = 3;
	static const int Rook = 4;
	static const int Queen = 5;
	static const int King = 6;

	static const int White = 0;
	static const int Black = 8;

	static const int WhitePawn = White | Pawn;
	static const int WhiteKnight = White | Knight;
	static const int WhiteBishop = White | Bishop;
	static const int WhiteRook = White | Rook;
	static const int WhiteQueen = White | Queen;
	static const int WhiteKing = White | King;

	static const int BlackPawn = Black | Pawn;
	static const int BlackKnight = Black | Knight;
	static const int BlackBishop = Black | Bishop;
	static const int BlackRook = Black | Rook;
	static const int BlackQueen = Black | Queen;
	static const int BlackKing = Black | King;
	
	static bool IsColor(int piece, int color);
	static bool IsSlidingPiece(int piece);
	static int getIndex(int piece);
	static int getPiece(int piece);
	static bool MovesInDirection(int piece, int dir);
	static int MakePiece(int color, int type);
	static int GetOpponentColor(int color);
	static int GetColor(int piece);
	static int oppositeColor(int color);
	static bool IsKnight(int piece);
	static bool IsPawn(int piece);
	static bool IsKing(int piece);
	static bool IsRook(int piece);
	static bool IsBishop(int piece);
	static bool IsQueen(int piece);
	static bool IsWhite(int piece);
	static bool IsBlack(int piece);

	static int Type(int piece);
	static void fenToBoard(const string& fen1, int Square[64]);
};