#include <SFML/System/Clock.hpp>
#include "Renderer.hpp"
#include "Table.hpp"

#include <iostream>


int main(){
    Renderer renderer(1920, 1080);
    Table table(1200, 600);

    // Initialisation de l'état au menu principal
    AppState currentState = APP_MENU;

    sf::Clock clock;

    while (renderer.isOpen()){
        // Gestion des entrées et mise à jour de l'état de l'application
        currentState = renderer.handleInput(table, currentState);

        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; 
        
        // Mise à jour de la physique (sub-stepping) uniquement à l'état PLAYING
        if (currentState == APP_PLAYING) {
            int subSteps = 4;
            float subDt = dt / subSteps;
            for(int s=0; s<subSteps; ++s){
                table.update(subDt);
            }
            
            // Vérification de la condition de fin de partie
            if (table.getGameStatus() != PLAYING) {
                currentState = APP_GAME_OVER; // Transition vers l'écran de résultats
            }
        }

        // Rendu de l'interface graphique selon l'état courant
        if (currentState == APP_MENU) {
            renderer.renderMenu();
        } else if (currentState == APP_PLAYING) {
            renderer.render(table);
        }
        // Rendu de l'écran de pause
        else if (currentState == APP_PAUSED) {
            renderer.renderPause(table);
        }
        // Rendu de l'écran de fin de partie
        else if (currentState == APP_GAME_OVER) {
            renderer.renderGameOver(table); 
        }
    }
    return 0;
}