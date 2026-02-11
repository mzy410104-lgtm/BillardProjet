#pragma once
#include <SFML/Graphics.hpp>
#include "Table.hpp"

class Renderer {
private:
    sf::RenderWindow window;
    bool debugMode;
public:
    Renderer(int w, int h);
    bool isOpen() const;
    //处理用户输入
    void handleInput();
    //渲染函数
    void render(const Table& table); 
    void toggleDebugMode(){debugMode = !debugMode;}
};
