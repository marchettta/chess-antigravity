#ifndef AI_HPP
#define AI_HPP

#include "Board.hpp"
#include <random>

struct Move {
    int startX, startY;
    int endX, endY;
    int score;
};

class AI {
public:
    AI(PieceColor color);
    Move getBestMove(Board board, int depth);

private:
    int evaluate(const Board& board);
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);
    PieceColor aiColor;
    std::mt19937 rng;
};

#endif
