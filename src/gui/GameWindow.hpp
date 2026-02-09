#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <SFML/Graphics.hpp>
#include "Board.hpp"
#include "AI.hpp"
#include <map>

class GameWindow {
public:
    GameWindow();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void loadTextures();
    void handleMouseClick(int x, int y);

    sf::RenderWindow window;
    Board board;
    std::map<std::string, sf::Texture> textures;
    sf::Font font;
    bool fontLoaded = false;
    
    int selectedX = -1;
    int selectedY = -1;
    bool pieceSelected = false;
    
    bool gameOver = false;
    bool isStalemate = false;
    PieceColor winner = PieceColor::None;
    
    AI blackAI;
};

#endif
