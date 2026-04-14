#pragma once
#include <SFML/Graphics.hpp>
#include "Table.hpp"

// États de l'application
enum AppState {
    APP_MENU,       // Menu principal
    APP_PLAYING,    // En jeu
    APP_PAUSED,     // En pause
    APP_GAME_OVER   // Écran de fin (résultats)
};

class Renderer {
private:
    sf::RenderWindow window;
    bool debugMode;

    // État de visée
    bool isAiming;

    // Décalage du point d'impact sur la bille blanche
    sf::Vector2f hitOffset;

    // Ressources de l'interface utilisateur (UI)
    sf::Font font;          // Police de caractères
    sf::Text playerText;    // Affichage du nom du joueur
    sf::Text stateText;     // Affichage de l'état de la table (groupes)
    sf::Text messageText;   // Affichage des messages temporaires (ex: bille en main)

    // Ressources de textures
    sf::Texture tableTex;       // Texture du tapis
    sf::Texture borderTex;      // Texture de la bordure
    sf::Texture ballWhiteTex;   // Bille blanche
    sf::Texture ballBlackTex;   // Bille noire
    sf::Texture ballRedTex;     // Bille rouge
    sf::Texture ballBlueTex;    // Bille bleue

    // Indicateur de chargement des textures (fallback sur couleurs unies si échec)
    bool texturesLoaded;

public:
    Renderer(int w, int h);
    bool isOpen() const;

    // Gestion des entrées et mise à jour de l'état
    AppState handleInput(Table& table, AppState currentState);

    // Rendu de la table de jeu
    void render(const Table& table); 
    void toggleDebugMode(){debugMode = !debugMode;}

    // Rendu du menu principal
    void renderMenu();
    
    // Rendu de l'écran de pause et de l'écran de fin
    void renderPause(const Table& table);
    void renderGameOver(const Table& table);

private:
    // Initialisation de l'interface utilisateur
    void initUI();

    // Chargement des ressources graphiques
    void loadAssets();
    
};