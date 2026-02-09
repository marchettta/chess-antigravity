#include "Board.hpp"
#include <cmath>

Board::Board() {
    reset();
}

void Board::reset() {
    turn = PieceColor::White;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            squares[i][j] = {PieceType::None, PieceColor::None};
        }
    }

    // Set up pawns
    for (int i = 0; i < 8; i++) {
        squares[i][1] = {PieceType::Pawn, PieceColor::Black};
        squares[i][6] = {PieceType::Pawn, PieceColor::White};
    }

    // Set up back rows
    PieceColor colors[2] = {PieceColor::Black, PieceColor::White};
    int rows[2] = {0, 7};
    for (int k = 0; k < 2; k++) {
        int r = rows[k];
        PieceColor c = colors[k];
        squares[0][r] = squares[7][r] = {PieceType::Rook, c};
        squares[1][r] = squares[6][r] = {PieceType::Knight, c};
        squares[2][r] = squares[5][r] = {PieceType::Bishop, c};
        squares[3][r] = {PieceType::Queen, c};
        squares[4][r] = {PieceType::King, c};
    }

    whiteKingMoved = blackKingMoved = false;
    whiteRookAMoved = whiteRookHMoved = false;
    blackRookAMoved = blackRookHMoved = false;
}

Piece Board::getPiece(int x, int y) const {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return {PieceType::None, PieceColor::None};
    return squares[x][y];
}

bool Board::isLegalMove(int startX, int startY, int endX, int endY) const {
    if (startX < 0 || startX >= 8 || startY < 0 || startY >= 8) return false;
    if (endX < 0 || endX >= 8 || endY < 0 || endY >= 8) return false;
    
    Piece startPiece = squares[startX][startY];
    if (startPiece.color != turn) return false;
    
    Piece endPiece = squares[endX][endY];
    if (endPiece.color == turn) return false;

    bool basicMoveLegal = false;
    switch (startPiece.type) {
        case PieceType::Pawn: basicMoveLegal = isValidPawnMove(startX, startY, endX, endY); break;
        case PieceType::Knight: basicMoveLegal = isValidKnightMove(startX, startY, endX, endY); break;
        case PieceType::Bishop: basicMoveLegal = isValidBishopMove(startX, startY, endX, endY); break;
        case PieceType::Rook: basicMoveLegal = isValidRookMove(startX, startY, endX, endY); break;
        case PieceType::Queen: basicMoveLegal = isValidQueenMove(startX, startY, endX, endY); break;
        case PieceType::King: basicMoveLegal = isValidKingMove(startX, startY, endX, endY); break;
        default: basicMoveLegal = false;
    }

    if (!basicMoveLegal) return false;

    // Simulate move to check for king safety (except for castling which has its own checks)
    if (startPiece.type == PieceType::King && std::abs(endX - startX) == 2) {
        return true; // isValidKingMove already handled check safety for castling
    }

    Board tempBoard = *this;
    // Perform standard move on temp board (ignore castling rook move here as we already validated it)
    tempBoard.squares[endX][endY] = tempBoard.squares[startX][startY];
    tempBoard.squares[startX][startY] = {PieceType::None, PieceColor::None};
    
    return !tempBoard.isInCheck(startPiece.color);
}

bool Board::movePiece(int startX, int startY, int endX, int endY) {
    if (isLegalMove(startX, startY, endX, endY)) {
        Piece p = squares[startX][startY];
        
        // Handle castling rook move
        if (p.type == PieceType::King && std::abs(endX - startX) == 2) {
            int rookStartY = startY;
            int rookStartX = (endX > startX) ? 7 : 0;
            int rookEndX = (endX > startX) ? 5 : 3;
            squares[rookEndX][rookStartY] = squares[rookStartX][rookStartY];
            squares[rookStartX][rookStartY] = {PieceType::None, PieceColor::None};
        }

        // Update movement flags
        if (p.type == PieceType::King) {
            if (p.color == PieceColor::White) whiteKingMoved = true;
            else blackKingMoved = true;
        }
        if (p.type == PieceType::Rook) {
            if (startX == 0 && startY == 7) whiteRookAMoved = true;
            if (startX == 7 && startY == 7) whiteRookHMoved = true;
            if (startX == 0 && startY == 0) blackRookAMoved = true;
            if (startX == 7 && startY == 0) blackRookHMoved = true;
        }

        squares[endX][endY] = squares[startX][startY];
        squares[startX][startY] = {PieceType::None, PieceColor::None};
        turn = (turn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        return true;
    }
    return false;
}

bool Board::isValidPawnMove(int startX, int startY, int endX, int endY) const {
    int dir = (squares[startX][startY].color == PieceColor::White) ? -1 : 1;
    int dx = endX - startX;
    int dy = endY - startY;

    if (dx == 0) {
        if (dy == dir && squares[endX][endY].type == PieceType::None) return true;
        if (dy == 2 * dir && ((startY == 1 && dir == 1) || (startY == 6 && dir == -1)) && 
            squares[endX][endY].type == PieceType::None && squares[startX][startY + dir].type == PieceType::None) return true;
    } else if (std::abs(dx) == 1 && dy == dir) {
        if (squares[endX][endY].type != PieceType::None && squares[endX][endY].color != squares[startX][startY].color) return true;
    }
    return false;
}

bool Board::isValidKnightMove(int startX, int startY, int endX, int endY) const {
    int dx = std::abs(endX - startX);
    int dy = std::abs(endY - startY);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

bool Board::isValidBishopMove(int startX, int startY, int endX, int endY) const {
    if (std::abs(endX - startX) != std::abs(endY - startY)) return false;
    return isPathClear(startX, startY, endX, endY);
}

bool Board::isValidRookMove(int startX, int startY, int endX, int endY) const {
    if (startX != endX && startY != endY) return false;
    return isPathClear(startX, startY, endX, endY);
}

bool Board::isValidQueenMove(int startX, int startY, int endX, int endY) const {
    if (startX != endX && startY != endY && std::abs(endX - startX) != std::abs(endY - startY)) return false;
    return isPathClear(startX, startY, endX, endY);
}

bool Board::isValidKingMove(int startX, int startY, int endX, int endY) const {
    int dx = std::abs(endX - startX);
    int dy = std::abs(endY - startY);
    
    if (dx <= 1 && dy <= 1) return true;

    // Castling
    if (dy == 0 && dx == 2) {
        PieceColor color = squares[startX][startY].color;
        bool kingMoved = (color == PieceColor::White) ? whiteKingMoved : blackKingMoved;
        if (kingMoved) return false;
        if (isInCheck(color)) return false;

        if (endX > startX) { // King-side
            bool rookMoved = (color == PieceColor::White) ? whiteRookHMoved : blackRookHMoved;
            if (rookMoved) return false;
            if (!isPathClear(startX, startY, 7, startY)) return false;
            if (isSquareAttacked(startX + 1, startY, (color == PieceColor::White ? PieceColor::Black : PieceColor::White))) return false;
        } else { // Queen-side
            bool rookMoved = (color == PieceColor::White) ? whiteRookAMoved : blackRookAMoved;
            if (rookMoved) return false;
            if (!isPathClear(startX, startY, 0, startY)) return false;
            if (isSquareAttacked(startX - 1, startY, (color == PieceColor::White ? PieceColor::Black : PieceColor::White))) return false;
        }
        return true;
    }

    return false;
}

bool Board::isSquareAttacked(int x, int y, PieceColor attackerColor) const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = squares[i][j];
            if (p.color == attackerColor) {
                // Simplified check: we use the move validation logic
                // Temporarily swap turn to use isLegalMove logic for checking attacks
                Board* mutableThis = const_cast<Board*>(this);
                PieceColor originalTurn = mutableThis->turn;
                mutableThis->turn = attackerColor;
                bool canReach = false;
                
                // For pawns, we only care about diagonals
                if (p.type == PieceType::Pawn) {
                    int dir = (attackerColor == PieceColor::White) ? -1 : 1;
                    if (std::abs(i - x) == 1 && (y - j) == dir) canReach = true;
                } else {
                    // Check other pieces. Note: we don't call isLegalMove directly to avoid recursion
                    // we'll implement a simple attack version
                    int dx = std::abs(x - i);
                    int dy = std::abs(y - j);
                    switch (p.type) {
                        case PieceType::Knight: if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1)) canReach = true; break;
                        case PieceType::Bishop: if (dx == dy && isPathClear(i, j, x, y)) canReach = true; break;
                        case PieceType::Rook: if ((i == x || j == y) && isPathClear(i, j, x, y)) canReach = true; break;
                        case PieceType::Queen: if ((dx == dy || i == x || j == y) && isPathClear(i, j, x, y)) canReach = true; break;
                        case PieceType::King: if (dx <= 1 && dy <= 1) canReach = true; break;
                        default: break;
                    }
                }

                mutableThis->turn = originalTurn;
                if (canReach) return true;
            }
        }
    }
    return false;
}

bool Board::isInCheck(PieceColor color) const {
    int kx = -1, ky = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (squares[i][j].type == PieceType::King && squares[i][j].color == color) {
                kx = i; ky = j; break;
            }
        }
    }
    if (kx == -1) return false;
    return isSquareAttacked(kx, ky, (color == PieceColor::White ? PieceColor::Black : PieceColor::White));
}

bool Board::hasLegalMoves(PieceColor color) const {
    // Save current turn state
    PieceColor originalTurn = turn;
    Board* mutableThis = const_cast<Board*>(this);
    mutableThis->turn = color;

    for (int sx = 0; sx < 8; sx++) {
        for (int sy = 0; sy < 8; sy++) {
            if (squares[sx][sy].color == color) {
                for (int ex = 0; ex < 8; ex++) {
                    for (int ey = 0; ey < 8; ey++) {
                        if (isLegalMove(sx, sy, ex, ey)) {
                            mutableThis->turn = originalTurn;
                            return true;
                        }
                    }
                }
            }
        }
    }
    mutableThis->turn = originalTurn;
    return false;
}

bool Board::isCheckmate(PieceColor color) const {
    return isInCheck(color) && !hasLegalMoves(color);
}

bool Board::isStalemate(PieceColor color) const {
    return !isInCheck(color) && !hasLegalMoves(color);
}
bool Board::isPathClear(int startX, int startY, int endX, int endY) const {
    int dx = (endX > startX) ? 1 : (endX < startX ? -1 : 0);
    int dy = (endY > startY) ? 1 : (endY < startY ? -1 : 0);
    int x = startX + dx;
    int y = startY + dy;
    while (x != endX || y != endY) {
        if (squares[x][y].type != PieceType::None) return false;
        x += dx;
        y += dy;
    }
    return true;
}
