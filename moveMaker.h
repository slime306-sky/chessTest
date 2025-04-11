#include "Board.h"
#include "moveGenerator.h"

#pragma once
class moveMaker {
private:
	moveGenerator generator;
public:
	void isGameOver(Board& board);
};