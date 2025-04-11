#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class Board;

class Piece {
protected:
    bool isWhite;

public:
    Piece(bool isWhite) : isWhite(isWhite) {}
    virtual ~Piece() {}
    virtual bool isValidMove(int fromX, int fromY, int toX, int toY, const Board& board) const = 0;
    virtual void display() const = 0;
    bool getColor() const { return isWhite; }
};

class Board {
private:
    vector<vector<Piece*>> board;

public:
    Board(int boardArr[8][8]) {
        board.resize(8, vector<Piece*>(8, nullptr));
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = createPiece(boardArr[i][j]);
            }
        }
    }

    ~Board() {
        for (auto& row : board) {
            for (auto& piece : row) {
                delete piece;
            }
        }
    }

    Piece* createPiece(int val) {
        if (val == -1) return nullptr; // Empty square

        bool isWhite = (val < 6);
        switch (val % 6) {
        case 0: return new King(isWhite);
        case 1: return new Queen(isWhite);
        case 2: return new Rook(isWhite);
        case 3: return new Knight(isWhite);
        case 4: return new Bishop(isWhite);
        case 5: return new Pawn(isWhite);
        }
        return nullptr;
    }

    Piece* getPiece(int x, int y) const {
        return (x >= 0 && x < 8 && y >= 0 && y < 8) ? board[y][x] : nullptr;
    }

    void setPiece(int x, int y, Piece* piece) {
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            board[y][x] = piece;
        }
    }

    bool isPathClear(int fromX, int fromY, int toX, int toY) const {
        int dx = (toX > fromX) ? 1 : (toX < fromX) ? -1 : 0;
        int dy = (toY > fromY) ? 1 : (toY < fromY) ? -1 : 0;
        int x = fromX + dx;
        int y = fromY + dy;
        while (x != toX || y != toY) {
            if (getPiece(x, y) != nullptr) return false;
            x += dx;
            y += dy;
        }
        return true;
    }
};

class Rook : public Piece {
public:
    Rook(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board& board) const override {
        return (fromX == toX || fromY == toY) && board.isPathClear(fromX, fromY, toX, toY);
    }
    void display() const override { cout << (isWhite ? "R" : "r") << " "; }
};

class Bishop : public Piece {
public:
    Bishop(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board& board) const override {
        return abs(fromX - toX) == abs(fromY - toY) && board.isPathClear(fromX, fromY, toX, toY);
    }
    void display() const override { cout << (isWhite ? "B" : "b") << " "; }
};

class Queen : public Piece {
public:
    Queen(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board& board) const override {
        return (fromX == toX || fromY == toY || abs(fromX - toX) == abs(fromY - toY)) && board.isPathClear(fromX, fromY, toX, toY);
    }
    void display() const override { cout << (isWhite ? "Q" : "q") << " "; }
};

class Knight : public Piece {
public:
    Knight(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board&) const override {
        return (abs(toX - fromX) == 2 && abs(toY - fromY) == 1) || (abs(toX - fromX) == 1 && abs(toY - fromY) == 2);
    }
    void display() const override { cout << (isWhite ? "N" : "n") << " "; }
};

class King : public Piece {
public:
    King(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board&) const override {
        return abs(toX - fromX) <= 1 && abs(toY - fromY) <= 1;
    }
    void display() const override { cout << (isWhite ? "K" : "k") << " "; }
};

class Pawn : public Piece {
public:
    Pawn(bool isWhite) : Piece(isWhite) {}
    bool isValidMove(int fromX, int fromY, int toX, int toY, const Board& board) const override {
        int direction = isWhite ? 1 : -1;
        if (fromX == toX && board.getPiece(toX, toY) == nullptr) {
            if (toY - fromY == direction) return true;
            if ((isWhite && fromY == 1) || (!isWhite && fromY == 6)) {
                if (toY - fromY == 2 * direction && board.getPiece(fromX, fromY + direction) == nullptr) return true;
            }
        }
        if (abs(toX - fromX) == 1 && toY - fromY == direction && board.getPiece(toX, toY) != nullptr) {
            return true;
        }
        return false;
    }
    void display() const override { cout << (isWhite ? "P" : "p") << " "; }
};




int main() {
    int boardArr[8][8] = {
        {2, 3, 4, 1, 0, 4, 3, 2},
        {5, 5, 5, 5, 5, 5, 5, 5},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {11, 11, 11, 11, 11, 11, 11, 11},
        {8, 9, 10, 7, 6, 10, 9, 8}
    };

    Board board(boardArr);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.getPiece(i, j)) board.getPiece(i, j)->display();
            else cout << ". ";
        }
        cout << endl;
    }
    return 0;
}
