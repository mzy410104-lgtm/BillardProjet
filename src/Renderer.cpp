#include <SFML/Graphics.hpp>
#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer(int w,int h) : window(sf::VideoMode(w,h),"Billard"),debugMode(false){
    window.setFramerateLimit(60);
}

bool Renderer::isOpen() const{
    return window.isOpen();
}

void Renderer::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed){
            if (event.key.code == sf::Keyboard::D){
                toggleDebugMode();
                std::cout << "Debug Mode: " << (debugMode ? "ON" : "OFF") <<std::endl;
            }
        }
    }
}

void Renderer::render(const Table& table){
    window.clear(sf::Color(0, 100, 0));
    //获取桌子上的球
    const auto& balls = table.getBalls();

    for (const auto& ball : balls) {
        //物理数据映射到图形数据
        float r = ball.getRadium();
        Vector2D pos = ball.getPosition();

        sf::CircleShape shape(r);
        shape.setFillColor(sf::Color::White);
        //将圆心对其
        shape.setOrigin(r, r);
        shape.setPosition(pos.x, pos.y);

        window.draw(shape);

        if (debugMode){
            Vector2D vel = ball.getVelocity();
            float speed = vel.length();

            //如果速度太小，不画线
            if (speed >5.0f){
                sf::Vector2f start(pos.x,pos.y);
                sf::Vector2f end(pos.x + vel.x *0.3f,pos.y + vel.y *0.3f);
                sf::Vertex line[] = {
                    sf::Vertex(start,sf::Color::Red),
                    sf::Vertex(end,sf::Color::Red)
                };
                window.draw(line,2,sf::Lines);
            }
        }
    }
    window.display();
}