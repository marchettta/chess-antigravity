#include "AI.hpp"
#include <algorithm>
#include <limits>
#include <chrono>

// Piece-Square Tables (from the perspective of White, will be flipped for Black)
const int pawnPST[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

const int knightPST[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int bishopPST[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int rookPST[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

const int queenPST[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int kingPST[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

AI::AI(PieceColor color) : aiColor(color) {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

int AI::evaluate(const Board& board) {
    int score = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = board.getPiece(i, j);
            if (p.type == PieceType::None) continue;
            
            int value = 0;
            int posBonus = 0;
            
            // Flip coordinates for Black PST
            int r = (p.color == PieceColor::White) ? j : 7 - j;
            int c = i;

            switch (p.type) {
                case PieceType::Pawn: 
                    value = 100; 
                    posBonus = pawnPST[r][c];
                    break;
                case PieceType::Knight: 
                    value = 320; 
                    posBonus = knightPST[r][c];
                    break;
                case PieceType::Bishop: 
                    value = 330; 
                    posBonus = bishopPST[r][c];
                    break;
                case PieceType::Rook: 
                    value = 500; 
                    posBonus = rookPST[r][c];
                    break;
                case PieceType::Queen: 
                    value = 900; 
                    posBonus = queenPST[r][c];
                    break;
                case PieceType::King: 
                    value = 20000; 
                    posBonus = kingPST[r][c];
                    break;
                default: break;
            }
            
            if (p.color == aiColor) score += (value + posBonus);
            else score -= (value + posBonus);
        }
    }
    return score;
}

Move AI::getBestMove(Board board, int depth) {
    int bestScore = std::numeric_limits<int>::min();
    std::vector<Move> bestMoves;

    for (int sx = 0; sx < 8; sx++) {
        for (int sy = 0; sy < 8; sy++) {
            if (board.getPiece(sx, sy).color != aiColor) continue;
            for (int ex = 0; ex < 8; ex++) {
                for (int ey = 0; ey < 8; ey++) {
                    if (board.isLegalMove(sx, sy, ex, ey)) {
                        Board nextBoard = board;
                        nextBoard.movePiece(sx, sy, ex, ey);
                        int score = minimax(nextBoard, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
                        
                        if (score > bestScore) {
                            bestScore = score;
                            bestMoves.clear();
                            bestMoves.push_back({sx, sy, ex, ey, bestScore});
                        } else if (score == bestScore) {
                            bestMoves.push_back({sx, sy, ex, ey, bestScore});
                        }
                    }
                }
            }
        }
    }
    
    if (bestMoves.empty()) return {-1, -1, -1, -1, 0};
    
    std::uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}

int AI::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) return evaluate(board);

    PieceColor currentTurn = board.getTurn();
    
    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (int sx = 0; sx < 8; sx++) {
            for (int sy = 0; sy < 8; sy++) {
                if (board.getPiece(sx, sy).color != currentTurn) continue;
                for (int ex = 0; ex < 8; ex++) {
                    for (int ey = 0; ey < 8; ey++) {
                        if (board.isLegalMove(sx, sy, ex, ey)) {
                            Board nextBoard = board;
                            nextBoard.movePiece(sx, sy, ex, ey);
                            int eval = minimax(nextBoard, depth - 1, alpha, beta, false);
                            maxEval = std::max(maxEval, eval);
                            alpha = std::max(alpha, eval);
                            if (beta <= alpha) break;
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (int sx = 0; sx < 8; sx++) {
            for (int sy = 0; sy < 8; sy++) {
                if (board.getPiece(sx, sy).color != currentTurn) continue;
                for (int ex = 0; ex < 8; ex++) {
                    for (int ey = 0; ey < 8; ey++) {
                        if (board.isLegalMove(sx, sy, ex, ey)) {
                            Board nextBoard = board;
                            nextBoard.movePiece(sx, sy, ex, ey);
                            int eval = minimax(nextBoard, depth - 1, alpha, beta, true);
                            minEval = std::min(minEval, eval);
                            beta = std::min(beta, eval);
                            if (beta <= alpha) break;
                        }
                    }
                }
            }
        }
        return minEval;
    }
}
