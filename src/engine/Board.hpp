#ifndef BOARD_HPP
#define BOARD_HPP

#include "Piece.hpp"
#include <vector>
#include <string>

class Board {
public:
    Board();
    void reset();
    Piece getPiece(int x, int y) const;
    bool movePiece(int startX, int startY, int endX, int endY);
    bool isLegalMove(int startX, int startY, int endX, int endY) const;
    bool isSquareAttacked(int x, int y, PieceColor attackerColor) const;
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;
    bool hasLegalMoves(PieceColor color) const;
    
    PieceColor getTurn() const { return turn; }

private:
    Piece squares[8][8];
    PieceColor turn;
    
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteRookAMoved = false;
    bool whiteRookHMoved = false;
    bool blackRookAMoved = false;
    bool blackRookHMoved = false;
    
    bool isValidPawnMove(int startX, int startY, int endX, int endY) const;
    bool isValidKnightMove(int startX, int startY, int endX, int endY) const;
    bool isValidBishopMove(int startX, int startY, int endX, int endY) const;
    bool isValidRookMove(int startX, int startY, int endX, int endY) const;
    bool isValidQueenMove(int startX, int startY, int endX, int endY) const;
    bool isValidKingMove(int startX, int startY, int endX, int endY) const;
    
    bool isPathClear(int startX, int startY, int endX, int endY) const;
};

#endif
