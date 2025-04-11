#pragma once


#include "Board.h"
#include "useFullStuff.h"
#include "Piece.h"
#include "Renderer.h"

using namespace std;

class BoardUI {
	private:
		SDL_Texture* textures[12] = { nullptr };
		Board* logicBoard;
		bool isFlipped = false;
	public:
		BoardUI(Board* b);
		bool isFlip();
		void Flip();
		void draw(SDL_Renderer* renderer, int selectedSquare = -1, const vector<Move>& legalMoves = {});
		void selectedSquareHighLight(SDL_Renderer* renderer, int selectedSquare = -1);
		void highLightLegalMoves(SDL_Renderer* renderer, int selectedSquare = -1, const vector<Move>& legalMoves = {});
		void loadTextures(SDL_Renderer* renderer);
		SDL_Texture* loadPieceTexture(SDL_Renderer* renderer, const string& filePath);
		void renderPieces(SDL_Renderer* renderer);
		int showPromotionWindow(SDL_Renderer* renderer, int color);

};