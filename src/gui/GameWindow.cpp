#include "GameWindow.hpp"
#include <iostream>

GameWindow::GameWindow() : window(sf::VideoMode(800, 800), "Ajedrez C++"), blackAI(PieceColor::Black) {
    loadTextures();
}

void GameWindow::loadTextures() {
    // For now, we will draw shapes if textures are missing, but let's define the paths.
    // Piece names: wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
    std::string pieces[] = {"wP", "wN", "wB", "wR", "wQ", "wK", "bP", "bN", "bB", "bR", "bQ", "bK"};
    std::string searchPaths[] = {"assets/", "../assets/", "./"};
    
    for (const std::string& p : pieces) {
        sf::Texture tex;
        bool loaded = false;
        for (const std::string& path : searchPaths) {
            std::string fullPath = path + p + ".png";
            if (tex.loadFromFile(fullPath)) {
                textures[p] = tex;
                loaded = true;
                std::cout << "Successfully loaded: " << fullPath << std::endl;
                break;
            }
        }
        if (!loaded) {
            std::cerr << "Failed to find texture for: " << p << std::endl;
        }
    }

    // Try to load a system font
    std::string fonts[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
    };
    for (const std::string& f : fonts) {
        if (font.loadFromFile(f)) {
            fontLoaded = true;
            break;
        }
    }
}

void GameWindow::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void GameWindow::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}

void GameWindow::handleMouseClick(int x, int y) {
    if (gameOver) return;

    int gridX = x / 100;
    int gridY = y / 100;

    if (!pieceSelected) {
        Piece p = board.getPiece(gridX, gridY);
        if (p.type != PieceType::None && p.color == board.getTurn()) {
            selectedX = gridX;
            selectedY = gridY;
            pieceSelected = true;
        }
    } else {
        if (board.movePiece(selectedX, selectedY, gridX, gridY)) {
            // Player moved, now AI moves
            render(); // Show player move
            
            if (board.getTurn() == PieceColor::Black) {
                if (board.isCheckmate(PieceColor::Black)) {
                    gameOver = true;
                    winner = PieceColor::White;
                } else if (board.isStalemate(PieceColor::Black)) {
                    gameOver = true;
                    isStalemate = true;
                } else {
                    Move aiMove = blackAI.getBestMove(board, 3);
                    if (aiMove.startX != -1) {
                        board.movePiece(aiMove.startX, aiMove.startY, aiMove.endX, aiMove.endY);
                        // Check after AI move
                        if (board.isCheckmate(PieceColor::White)) {
                            gameOver = true;
                            winner = PieceColor::Black;
                        } else if (board.isStalemate(PieceColor::White)) {
                            gameOver = true;
                            isStalemate = true;
                        }
                    }
                }
            }
        }
        pieceSelected = false;
    }
}

void GameWindow::update() {}

void GameWindow::render() {
    window.clear();

    // Draw board
    sf::RectangleShape square(sf::Vector2f(100.0f, 100.0f));
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 == 0) square.setFillColor(sf::Color(255, 206, 158));
            else square.setFillColor(sf::Color(209, 139, 71));
            
            if (pieceSelected && i == selectedX && j == selectedY) {
                square.setFillColor(sf::Color(130, 150, 105)); // Highlight selected
            }

            square.setPosition(i * 100, j * 100);
            window.draw(square);

            Piece p = board.getPiece(i, j);
            if (p.type != PieceType::None) {
                // If texture exists, draw it. Otherwise draw a circle/square as placeholder.
                std::string code = (p.color == PieceColor::White ? "w" : "b");
                switch(p.type) {
                    case PieceType::Pawn: code += "P"; break;
                    case PieceType::Knight: code += "N"; break;
                    case PieceType::Bishop: code += "B"; break;
                    case PieceType::Rook: code += "R"; break;
                    case PieceType::Queen: code += "Q"; break;
                    case PieceType::King: code += "K"; break;
                    default: break;
                }

                if (textures.count(code)) {
                    sf::Sprite sprite(textures[code]);
                    sprite.setPosition(i * 100, j * 100);
                    // Scale from 140x140 to 100x100
                    sprite.setScale(100.0f / 140.0f, 100.0f / 140.0f);
                    window.draw(sprite);
                } else {
                    sf::CircleShape circ(40);
                    circ.setFillColor(p.color == PieceColor::White ? sf::Color::White : sf::Color::Black);
                    circ.setOutlineThickness(2);
                    circ.setOutlineColor(sf::Color::Red);
                    circ.setPosition(i * 100 + 10, j * 100 + 10);
                    window.draw(circ);
                    
                    if (fontLoaded) {
                        sf::Text text;
                        text.setFont(font);
                        std::string label = code.substr(1);
                        text.setString(label);
                        text.setCharacterSize(40);
                        text.setFillColor(p.color == PieceColor::White ? sf::Color::Black : sf::Color::White);
                        // Center text
                        sf::FloatRect textRect = text.getLocalBounds();
                        text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
                        text.setPosition(sf::Vector2f(i * 100 + 50, j * 100 + 50));
                        window.draw(text);
                    }
                }
            }
        }
    }

    if (gameOver && fontLoaded) {
        sf::RectangleShape overlay(sf::Vector2f(800.0f, 800.0f));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        sf::Text text;
        text.setFont(font);
        if (isStalemate) {
            text.setString("STALEMATE!");
        } else {
            text.setString(winner == PieceColor::White ? "WHITE WINS!" : "BLACK WINS!");
        }
        text.setCharacterSize(80);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(400, 400));
        window.draw(text);
    }

    window.display();
}
