#include "BoardUI.h"
#include "moveGenerator.h"
#include "moveMaker.h"
#include <chrono>
#include "bitBoard.h"
//#include "notation.h"
//#include<iostream>

using namespace std;


bool isWatchingBots = false;
bool isSinglePlayer = false;
bool boardShouldFlip = false;
bool smartbot = false;
int botColor = Piece::Black;


class Agent {
private:
	moveGenerator generator;
	moveMaker checker;
	int positionsEvaluated = 0;
	int nodesSearched = 0;
public:

	void playRandomMove(Board& board) {
		vector<Move> legalMoves;
		//cout << "called function";
		legalMoves = generator.GenerateLegalMoves(&board);
		
		// Print all legal moves
		//for (const Move& move : legalMoves) {
		//	//printMove(move);
		//	std::cout << "-------------------\n";
		//}
		//cout << "in agent : " << legalMoves.size()<<endl;


		if (legalMoves.empty()) {
			// maybe return a null move, or just skip the turn, or throw an error
			//cerr << "No legal moves available!" << endl;
			checker.isGameOver(board);
			return;
		}
		int r = rand() % legalMoves.size();
		Move move = legalMoves[r];
		//cout << "Bot move: " << chosen.startSquare << " -> " << chosen.targetSquare << " to : " << chosen.promotionPiece << "\n";

		board.makeMove(move,board);
		board.recordPosition(board);
		checker.isGameOver(board);
		//int kingSq = board.findKingSquare(board.colorToMove);
		//bool inCheck = board.isSquareAttacked(kingSq, Piece::oppositeColor(board.colorToMove), board);
		//if (inCheck && legalMoves.empty()) {
		//	//move.san = notation::moveToSAN(move, true, true);
		//}
		//else if (inCheck) {
		//	//move.san = notation::moveToSAN(move, true, false);
		//}
		//else {
		//	//move.san = notation::moveToSAN(move, false, false);
		//}
		legalMoves.clear();
		gameMoves.push_back(move);
	}

	int evaluate(const Board& board) {
		int score = 0;

		const int pieceValues[7] = {
			0,    // None
			100,  // Pawn
			320,  // Knight
			330,  // Bishop
			500,  // Rook
			900,  // Queen
			20000 // King
		};

		for (int i = 0; i < 64; ++i) {
			int piece = board.Square[i];
			if (piece == Piece::None) continue;

			int type = Piece::Type(piece);
			int value = pieceValues[type];

			if (Piece::IsWhite(piece)) score += value;
			else if (Piece::IsBlack(piece)) score -= value;
		}

		return score;
	}

	int scoreMove(const Move& move, const Board& board) {
		int score = 0;

		const int pieceValues[7] = {
			0,    // None
			100,  // Pawn
			320,  // Knight
			330,  // Bishop
			500,  // Rook
			900,  // Queen
			20000 // King
		};

		// 1. Prioritize captures using MVV-LVA
		if (move.type == Move::Capture || move.type == Move::EnPassant) {
			int victimValue = pieceValues[Piece::Type(move.capturedPiece)];
			int attackerValue = pieceValues[Piece::Type(move.movedPiece)];

			score += (victimValue * 10 - attackerValue);  // MVV-LVA bonus
			score += 1000; // Additional boost to all captures
		}

		// 2. Promotion bonus (favor promoting to stronger pieces)
		if (move.type == Move::Promotion) {
			score += pieceValues[move.promotionPiece] + 800;
		}

		// 3. Castling bonuses (optional)
		if (move.type == Move::KingsideCastle || move.type == Move::QueensideCastle) {
			score += 50;  // small positional boost
		}

		// 4. Slight positional bonus to advancing pawns
		if (Piece::Type(move.movedPiece) == Piece::Pawn) {
			int rankDiff = std::abs(move.targetSquare / 8 - move.startSquare / 8);
			score += rankDiff * 5;
		}

		return score;
	}



	int alphaBeta(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
		nodesSearched++;

		if (depth == 0 || board.gameOver) {
			return evaluate(board);
		}

		std::vector<Move> moves = generator.GenerateLegalMoves(&board);

		// Partial sort: just sort the top few best moves
		const int SORT_COUNT = std::min(6, (int)moves.size());
		std::partial_sort(moves.begin(), moves.begin() + SORT_COUNT, moves.end(),
			[&](const Move& a, const Move& b) {
				return scoreMove(a, board) > scoreMove(b, board);
			});

		int moveCount = 0;

		if (maximizingPlayer) {
			int maxEval = -100000;
			for (Move& move : moves) {
				Board newBoard = board;
				newBoard.makeMove(move, newBoard);

				int eval;
				// Late Move Reduction (LMR)
				if (moveCount >= 4 && depth > 2) {
					eval = alphaBeta(newBoard, depth - 2, alpha, beta, false);  // reduced depth
				}
				else {
					eval = alphaBeta(newBoard, depth - 1, alpha, beta, false);  // normal
				}

				maxEval = std::max(maxEval, eval);
				alpha = std::max(alpha, eval);
				if (beta <= alpha) break;

				moveCount++;
			}
			return maxEval;
		}
		else {
			int minEval = 100000;
			for (Move& move : moves) {
				Board newBoard = board;
				newBoard.makeMove(move, newBoard);

				int eval;
				// Late Move Reduction (LMR)
				if (moveCount >= 4 && depth > 2) {
					eval = alphaBeta(newBoard, depth - 2, alpha, beta, true);  // reduced depth
				}
				else {
					eval = alphaBeta(newBoard, depth - 1, alpha, beta, true);  // normal
				}

				minEval = std::min(minEval, eval);
				beta = std::min(beta, eval);
				if (beta <= alpha) break;

				moveCount++;
			}
			return minEval;
		}
	}



	Move getBestMove(Board& board, int depth) {
		positionsEvaluated = 0;

		std::vector<Move> moves = generator.GenerateLegalMoves(&board);
		if (moves.empty()) {
			throw std::runtime_error("No legal moves available");
		}

		auto start = std::chrono::high_resolution_clock::now();

		Move bestMove = moves[0];
		bool isWhiteToMove = board.colorToMove == Piece::White;
		int bestEval = isWhiteToMove ? -100000 : 100000;

		for (Move& move : moves) {

			Board newBoard = board;
			newBoard.makeMove(move, newBoard);

			int eval = alphaBeta(newBoard, depth - 1, -100000, 100000, !isWhiteToMove);

			if (isWhiteToMove && eval > bestEval) {
				bestEval = eval;
				bestMove = move;
			}
			else if (!isWhiteToMove && eval < bestEval) {
				bestEval = eval;
				bestMove = move;
			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		double timeTaken = std::chrono::duration<double>(end - start).count();

		std::cout << "-----------------------------\n";
		std::cout << "Best Eval: " << bestEval << "\n";
		std::cout << "Positions Evaluated: " << nodesSearched << "\n";
		std::cout << "Time Taken: " << timeTaken << " seconds\n";
		std::cout << "-----------------------------\n";

		return bestMove;
	}

	int minimax(Board board, int depth, bool maximizingPlayer) {
		if (depth == 0 || board.gameOver) {
			positionsEvaluated++;
			return evaluate(board);
		}

		std::vector<Move> moves = generator.GenerateLegalMoves(&board);

		if (maximizingPlayer) {
			int maxEval = -100000;
			for (Move& move : moves) {
				Board newBoard = board;
				newBoard.makeMove(move, newBoard);
				int eval = minimax(newBoard, depth - 1, false);
				maxEval = std::max(maxEval, eval);
			}
			return maxEval;
		}
		else {
			int minEval = 100000;
			for (Move& move : moves) {
				Board newBoard = board;
				newBoard.makeMove(move, newBoard);
				int eval = minimax(newBoard, depth - 1, true);
				minEval = std::min(minEval, eval);
			}
			return minEval;
		}
	}


	Move getBestMoveUsingMinimax(Board& board, int depth) {
		positionsEvaluated = 0; // Reset counter

		std::vector<Move> moves = generator.GenerateLegalMoves(&board);
		if (moves.empty()) {
			throw std::runtime_error("No legal moves available");
		}

		auto start = std::chrono::high_resolution_clock::now();

		Move bestMove = moves[0];
		bool isWhiteToMove = board.colorToMove == Piece::White;
		int bestEval = isWhiteToMove ? -100000 : 100000;

		for (Move& move : moves) {
			Board newBoard = board;
			newBoard.makeMove(move, newBoard);
			int eval = minimax(newBoard, depth - 1, !isWhiteToMove);

			if (isWhiteToMove && eval > bestEval) {
				bestEval = eval;
				bestMove = move;
			}
			else if (!isWhiteToMove && eval < bestEval) {
				bestEval = eval;
				bestMove = move;
			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		double timeTaken = std::chrono::duration<double>(end - start).count();

		std::cout << "Best Move Eval: " << bestEval << "\n";
		std::cout << "Positions Evaluated: " << positionsEvaluated << "\n";
		std::cout << "Time Taken: " << timeTaken << " seconds\n";

		return bestMove;
	}






	void playBestMove(Board& board, int depth) {
		std::vector<Move> legalMoves = generator.GenerateLegalMoves(&board);

		if (legalMoves.empty()) {
			checker.isGameOver(board);
			return;
		}

		Move bestMove = getBestMove(board, depth);  // This uses alpha-beta

		//board.printMove(bestMove);

		board.makeMove(bestMove, board);
		board.recordPosition(board);
		checker.isGameOver(board);

		gameMoves.push_back(bestMove);
	}
};


class Game {
private:
	BoardUI boardUi;
	moveGenerator generator;
	int selectedSquare = -1;
	vector<Move> legalMoves;
	moveMaker checker;
	string resultMessage = "";
	Agent agent;



	int getSquareFromMouse(int x, int y) {
		int file = x / SQUARE_SIZE;
		int rank = y / SQUARE_SIZE;

		if (boardUi.isFlip()) {
			file = 7 - file;
			rank = rank; 
		}
		else {
			rank = 7 - rank;
		}

		return rank * 8 + file;
	}



public:
	Board board;

	Game(SDL_Renderer* renderer) : boardUi(&board) {
		boardUi.loadTextures(renderer);
		board.initZobrist();
		PrecomputedMoveData::Init();
		//initBitboardsFromBoard(board.Square);
		//printBitboard(whitePawns);
	}

	long long perft(int depth) {
		//cout << "depth"<<endl;
		if (depth == 0) return 1;

		vector<Move> moves = generator.GenerateLegalMoves(&board);

		long long nodes = 0;

		for (Move& move : moves) {
			//cout << "Making move: " << endl;
			board.makeMove(move, board);           // Apply move
			nodes += perft(depth - 1); // Go deeper
			//cout << "moved"<<endl;
			//cout << "Undoing move: " << endl;
			board.undoMove(move, board);               // Revert move
			//cout << "undo" << endl;
		}

		return nodes;
	}


	void perftDebug(int depth) {
		//cout << "depth";
		for (int i = 1; i <= depth; i++)
		{
			auto start = std::chrono::high_resolution_clock::now();

			long long total = 0;

			vector<Move> moves = generator.GenerateLegalMoves(&board);

			for (Move& move : moves) {
				board.makeMove(move, board);

				long long count = perft(i - 1);

				board.undoMove(move, board);

				//std::cout << move.startSquare << " -> " << move.targetSquare << ": " << count << std::endl;
				total += count;
			}

			std::cout << "Total nodes at depth " << i << ": " << total << std::endl << std::endl;
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			double seconds = duration.count() / 1000.0;  // convert ms to seconds

			std::cout << "Total nodes at depth " << i << ": " << total
				<< " (Time: " << seconds << " sec)" << std::endl << std::endl;
		}
	}

	void handleEvent(SDL_Event& e, SDL_Renderer* renderer) {
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
			int mouseX = e.button.x;
			int mouseY = e.button.y;
			int clickedSquare = getSquareFromMouse(mouseX, mouseY);
			handleClick(clickedSquare, renderer);
		}
	}

	void handleClick(int clickedSquare, SDL_Renderer* renderer) {
		int clickedPiece = board.Square[clickedSquare];

		if (selectedSquare == -1) {
			trySelectPiece(clickedSquare, clickedPiece);
		}
		else {
			tryMovePiece(clickedSquare, renderer);
		}
	}

	void trySelectPiece(int square, int piece) {
		if (Piece::IsColor(piece, board.colorToMove)) {
			legalMoves = generator.GenerateLegalMoves(&board);
			selectedSquare = square;

			//for (const auto& move : legalMoves)
			//	if (move.startSquare == selectedSquare)
			//		std::cout << move.startSquare << " -> " << move.targetSquare << "\n";
		}
	}

	void tryMovePiece(int targetSquare, SDL_Renderer* renderer) {
		bool moved = false;

		for (Move& move : legalMoves) {
			if (move.startSquare == selectedSquare && move.targetSquare == targetSquare) {
				int movingPiece = board.Square[move.startSquare];

				if (move.type == Move::Promotion && Piece::Type(movingPiece) == Piece::Pawn) {
					int promo = boardUi.showPromotionWindow(renderer, Piece::GetColor(movingPiece));
					move.promotionPiece = promo;
				}

				board.makeMove(move, board);
				gameMoves.push_back(move);
				moved = true;
				break;
			}
		}

		legalMoves.clear();
		selectedSquare = -1;

		if (moved) {
			board.recordPosition(board);
			legalMoves = generator.GenerateLegalMoves(&board);
			checker.isGameOver(board);  // Check for mate/stalemate
			legalMoves.clear();

			if (boardShouldFlip)
				boardUi.Flip();  // <--- flip the board after move
		}
	}


	void undomove() {
		if (gameMoves.empty())
			return;
		Move lastmove = gameMoves.back();
		board.undoMove(lastmove,board);
		boardUi.Flip();
		gameMoves.pop_back();
	}

	void update(SDL_Renderer* renderer) {


		if (!board.isOver()) {
			if (isWatchingBots) {
				SDL_Delay(200);
				agent.playRandomMove(board);
				board.recordPosition(board);
				checker.isGameOver(board);
			}
			else if (smartbot && isSinglePlayer && board.colorToMove == botColor) {
				SDL_Delay(200);
				agent.playBestMove(board, 4);
				board.recordPosition(board);
				checker.isGameOver(board);
			}
			else if (isSinglePlayer && board.colorToMove == botColor) {
				SDL_Delay(200);
				agent.playRandomMove(board);
				board.recordPosition(board);
				checker.isGameOver(board);
			}
		}



		// --- Drawing code ---
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		boardUi.draw(renderer, selectedSquare, legalMoves);
		boardUi.selectedSquareHighLight(renderer, selectedSquare);
		boardUi.renderPieces(renderer);
		boardUi.highLightLegalMoves(renderer, selectedSquare, legalMoves);

		SDL_RenderPresent(renderer);
	}

	void play2Player(bool flipBoard) {
		boardShouldFlip = flipBoard;
		isSinglePlayer = false;
		isWatchingBots = false;
	}
	void play1Player(bool flipBoard) {
		boardShouldFlip = flipBoard;
		isSinglePlayer = true;
		isWatchingBots = false;

		int choice = 0;
		cout << "Play as:\n1. White\n2. Black\nChoice: ";
		cin >> choice;
		botColor = (choice == 1) ? Piece::Black : Piece::White;
		if (choice == 2) {
			boardUi.Flip();
		}

		cout << "You play " << ((botColor == Piece::Black) ? "White" : "Black") << ". Bot plays "
			<< ((botColor == Piece::Black) ? "Black" : "White") << ".\n";
	}
	void play1PlayerSB(bool flipBoard) {
		boardShouldFlip = flipBoard;
		isSinglePlayer = true;
		smartbot = true;
		isWatchingBots = false;

		int choice = 0;
		cout << "Play as:\n1. White\n2. Black\nChoice: ";
		cin >> choice;
		botColor = (choice == 1) ? Piece::Black : Piece::White;
		if (choice == 2) {
			boardUi.Flip();
		}

		cout << "You play " << ((botColor == Piece::Black) ? "White" : "Black") << ". Bot plays "
			<< ((botColor == Piece::Black) ? "Black" : "White") << ".\n";
	}
	void play2Bot(bool flipBoard) {
		boardShouldFlip = flipBoard;
		isWatchingBots = true;
		isSinglePlayer = false;
	}




	void stating() {
		int choice = 0;

		while (true) {
			cout << "1. 2 player game (board will flip)\n";
			cout << "2. 2 player game (board will NOT flip)\n";
			cout << "3. 1 player game (bot will play random moves)\n";
			cout << "4. 1 player game (bot will play good moves)\n";
			cout << "5. Watch 2 bots play (board will NOT flip)\n";
			cout << "Enter your choice: ";
			cin >> choice;
		
			if (choice >= 1 && choice <= 5) break;
			cout << "Invalid choice. Please try again.\n";
		}

		switch (choice) {
		case 1:
			play2Player(true);
			break;
		case 2:
			play2Player(false);
			break;
		case 3:
			play1Player(false);
			break;
		case 4:
			play1PlayerSB(false);
			break;
		case 5:
			play2Bot(false);
			break;
		}

		cout << "Game is started.\n";
	}



};







int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Chess", WIDTH, HEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	Game game(renderer);
	srand(static_cast<unsigned>(time(0)));
	bool quit = false;
	SDL_Event e;

	game.stating();

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT)
				quit = true;

			if (!game.board.isOver()) {
				game.handleEvent(e, renderer);
			}

			if (e.type == SDL_EVENT_KEY_DOWN) {
				if (e.key.key == SDLK_SPACE) {
					game.undomove();
				}
				if (e.key.key == SDLK_P) {
					int depth = 5;
					//long long nodes = test.perft(depth);
					game.perftDebug(depth);
					//std::cout << "Perft(" << depth << ") = " << nodes << std::endl;
				}
			}

		}


		game.update(renderer);
	}
	cout << "game over";
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}