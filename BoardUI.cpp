#include "BoardUI.h"



BoardUI::BoardUI(Board* b) {
	logicBoard = b;
}

bool BoardUI::isFlip()
{
	return isFlipped;
}

void BoardUI::Flip() {
	isFlipped = !isFlipped;
}

void BoardUI::draw(SDL_Renderer* renderer, int selectedSquare, const vector<Move>& legalMoves) {
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			bool isLightSquare = (file + rank) % 2 != 0;
			Color squareColor = isLightSquare ? WHITE_COLOR : GRAY_COLOR;

			int displayRank = isFlipped ? rank : 7 - rank;
			int displayFile = isFlipped ? 7 - file : file;

			SquarePos pos = { displayFile * SQUARE_SIZE, displayRank * SQUARE_SIZE };
			Renderer::drawSquare(renderer, pos, squareColor);
		}
	}

	// Optional: highlight selected square or legal moves (not included here)
}



void BoardUI::selectedSquareHighLight(SDL_Renderer* renderer, int selectedSquare) {
	if (selectedSquare != -1) {
		int file = selectedSquare % 8;
		int rank = selectedSquare / 8;

		int displayFile = isFlipped ? 7 - file : file;
		int displayRank = isFlipped ? rank : 7 - rank;

		SquarePos pos = { displayFile * SQUARE_SIZE, displayRank * SQUARE_SIZE };
		Renderer::drawSquare(renderer, pos, CLICK_COLOR); // green overlay
	}
}


void BoardUI::highLightLegalMoves(SDL_Renderer* renderer, int selectedSquare, const vector<Move>& legalMoves) {
	for (const Move& move : legalMoves) {
		if (move.startSquare == selectedSquare) {
			int file = move.targetSquare % 8;
			int rank = move.targetSquare / 8;

			int displayFile = isFlipped ? 7 - file : file;
			int displayRank = isFlipped ? rank : 7 - rank;

			int centerX = displayFile * SQUARE_SIZE + SQUARE_SIZE / 2;
			int centerY = displayRank * SQUARE_SIZE + SQUARE_SIZE / 2;

			Renderer::drawCircle(renderer, centerX, centerY, SQUARE_SIZE / 6, SUGGESTIONCOLOR);
		}
	}
}


void BoardUI::loadTextures(SDL_Renderer* renderer) {
	string pieces[12] = {
		"WP.bmp", "WN.bmp", "WB.bmp", "WR.bmp", "WQ.bmp", "WK.bmp",
		"BP.bmp", "BN.bmp", "BB.bmp", "BR.bmp", "BQ.bmp", "BK.bmp"
	};

	for (int i = 0; i < 12; i++) {
		string filePath = "assets/" + pieces[i];
		SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
		if (!surface) {
			printf("Failed to load %s: %s\n", pieces[i].c_str(), SDL_GetError());
			continue;
		}
		textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_DestroySurface(surface);
	}
}

SDL_Texture* BoardUI::loadPieceTexture(SDL_Renderer* renderer, const std::string& filePath) {
	string filePath1 = "assets/" + filePath;
	cout << filePath1 << endl;
	SDL_Surface* surface = SDL_LoadBMP(filePath1.c_str());
	if (!surface) {
		std::cerr << "Failed to load image: " << filePath1 << std::endl;
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	if (!texture) {
		std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
	}

	return texture;
}

void BoardUI::renderPieces(SDL_Renderer* renderer) {
	for (int i = 0; i < 64; i++) {
		int piece = logicBoard->Square[i];
		if (piece == Piece::None) continue;

		int color = (piece & Piece::Black) ? 1 : 0;
		int type = piece & 7;

		int textureIndex = color * 6 + (type - 1);
		if (textureIndex < 0 || textureIndex >= 12) {
			printf("Invalid texture index %d for piece code %d\n", textureIndex, piece);
			continue;
		}

		int file = i % 8;
		int rank = i / 8;

		int displayRank = isFlipped ? rank : 7 - rank;
		int displayFile = isFlipped ? 7 - file : file;

		SDL_FRect rect = {
			displayFile * SQUARE_SIZE,
			displayRank * SQUARE_SIZE,
			SQUARE_SIZE, SQUARE_SIZE
		};

		SDL_RenderTexture(renderer, textures[textureIndex], NULL, &rect);
	}
}


int BoardUI::showPromotionWindow(SDL_Renderer* renderer, int color) {
	SDL_Window* promoWin = SDL_CreateWindow("Choose Promotion Piece",
		400, 100, 0);

	SDL_Renderer* promoRenderer = SDL_CreateRenderer(promoWin, NULL);

	// Load your piece textures here (replace with actual image loading)
	SDL_Texture* queenTex = loadPieceTexture(promoRenderer, color == Piece::White ? "WQ.bmp" : "BQ.bmp");
	SDL_Texture* rookTex = loadPieceTexture(promoRenderer, color == Piece::White ? "WR.bmp" : "BR.bmp");
	SDL_Texture* bishopTex = loadPieceTexture(promoRenderer, color == Piece::White ? "WB.bmp" : "BB.bmp");
	SDL_Texture* knightTex = loadPieceTexture(promoRenderer, color == Piece::White ? "WN.bmp" : "BN.bmp");

	SDL_FRect buttons[4];
	for (int i = 0; i < 4; i++) {
		buttons[i] = { static_cast<float>(i * 80), 0.0f, 100.0f, 100.0f };
	}

	SDL_Event e;
	bool running = true;
	int selected = -1;

	while (running) {
		SDL_RenderClear(promoRenderer);
		SDL_RenderTexture(promoRenderer, queenTex, NULL, &buttons[0]);
		SDL_RenderTexture(promoRenderer, rookTex, NULL, &buttons[1]);
		SDL_RenderTexture(promoRenderer, bishopTex, NULL, &buttons[2]);
		SDL_RenderTexture(promoRenderer, knightTex, NULL, &buttons[3]);
		SDL_RenderPresent(promoRenderer);

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				running = false;
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
				int x = e.button.x;
				for (int i = 0; i < 4; i++) {
					if (x >= buttons[i].x && x < buttons[i].x + 100) {
						selected = i;
						running = false;
						break;
					}
				}
			}
		}
	}

	SDL_DestroyTexture(queenTex);
	SDL_DestroyTexture(rookTex);
	SDL_DestroyTexture(bishopTex);
	SDL_DestroyTexture(knightTex);
	SDL_DestroyRenderer(promoRenderer);
	SDL_DestroyWindow(promoWin);

	// Return piece type
	switch (selected) {
	case 0: return Piece::Queen;
	case 1: return Piece::Rook;
	case 2: return Piece::Bishop;
	case 3: return Piece::Knight;
	default: return Piece::Queen; // fallback
	}
}