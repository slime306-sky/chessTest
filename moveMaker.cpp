#include "moveMaker.h"


void moveMaker::isGameOver(Board& board) {
	//cout << "checked if game is over or not"<<endl;

	vector<Move> nextMoves = generator.GenerateLegalMoves(&board);

	int kingSq = board.findKingSquare(board.colorToMove);
	bool inCheck = board.isSquareAttacked(kingSq, Piece::oppositeColor(board.colorToMove), board);

	// Check for checkmate or stalemate
	//cout << nextMoves.empty() << endl;
	if (inCheck && nextMoves.empty()) {
		board.gameOver = true;
		string winner = (board.colorToMove == Piece::White) ? "Black" : "White";
		string result = "Checkmate! " + winner + " wins!\n";
		cout << result;
		return;
	}
	else if (inCheck) {
		cout << "Check!\n";
	}
	else if (!inCheck && nextMoves.empty()) {
		board.gameOver = true;
		cout << "Stalemate! It's a draw!\n";
		return;
	}
	//if (nextMoves.empty()) {
	//	cout << "empty\n";
	//	if (inCheck) {
	//		gameOver = true;
	//		string winner = (board.colorToMove == Piece::White) ? "Black" : "White";
	//		string result = "Checkmate! " + winner + " wins!\n";
	//		cout << result;
	//		//string pgn = notation::toPGN(gameMoves, result);
	//		//cout << "PGN:\n" << pgn << endl;
	//		return;
	//	}
	//	else {
	//		gameOver = true;
	//		cout << "Stalemate! It's a draw!\n";
	//		//string pgn = notation::toPGN(gameMoves, "Stalemate! It's a draw!");
	//		//cout << "PGN:\n" << pgn << endl;
	//		return;
	//	}
	//}

	// Check for check (not game over, just notify)
	//if (inCheck) {
	//	cout << "Check!\n";
	//}

	// 50-move rule (100 halfmoves)
	if (board.halfmoveClock >= 100) {
		board.gameOver = true;
		cout << "Draw by fifty-move rule!\n";
		//string pgn = notation::toPGN(gameMoves, "Draw by fifty-move rule!");
		//cout << "PGN:\n" << pgn << endl;
		return;
	}

	// Insufficient material
	if (board.hasInsufficientMaterial()) {
		board.gameOver = true;
		cout << "Draw due to insufficient material!\n";
		//string pgn = notation::toPGN(gameMoves, "Draw due to insufficient material!");
		//cout << "PGN:\n" << pgn << endl;
		return;
	}

	//// Threefold repetition
	if (board.isThreefoldRepetition(board)) {
		board.gameOver = true;
		cout << "Draw by threefold repetition!\n";
		//string pgn = notation::toPGN(gameMoves, "Draw by threefold repetition!");
		//cout << "PGN:\n" << pgn << endl;
		return;
	}
}