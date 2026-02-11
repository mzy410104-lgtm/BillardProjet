#include <SFML/System/Clock.hpp>
#include "Renderer.hpp"
#include "Table.hpp"
#include <iostream>

int main(){
    Renderer renderer(1920,1080);
    Table table(800,600);

    //创建两个球
    Boule b1(Vector2D(100,300),30,1);
    b1.setVelocity(Vector2D(300.0f,0.0f));
    table.addBall(b1);

    Boule b2(Vector2D(500,340),30,1);
    b2.setVelocity(Vector2D(-300.0f,0.0f));
    table.addBall(b2);

    sf::Clock clock;

    //游戏进行
    while (renderer.isOpen()){
        renderer.handleInput();

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f){
            dt = 0.1f;
        }
        table.update(dt);
        renderer.render(table);
    }
    return 0;
}