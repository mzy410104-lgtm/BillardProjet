#pragma once
#include "Boule.hpp"
#include <vector>
#include <string>

// Énumération des joueurs
enum Player{
    PLAYER_1,
    PLAYER_2
};

// État d'attribution des groupes de billes
enum TableState{
    TABLE_OPEN,         // Table ouverte (groupes non assignés)
    P1_RED_P2_BLUE,     // Joueur 1 : Rouge, Joueur 2 : Bleu
    P1_BLUE_P2_RED      // Joueur 1 : Bleu, Joueur 2 : Rouge
};

// Statut global de la partie
enum GameStatus {
    PLAYING,            // Partie en cours
    P1_WINS,            // Victoire du Joueur 1
    P2_WINS,            // Victoire du Joueur 2
    P1_LOSE,            // Défaite du Joueur 1 (Pénalité fatale)
    P2_LOSE             // Défaite du Joueur 2 (Pénalité fatale)
};

// Structure représentant une poche
struct Pocket{
    Vector2D position;
    float radius;
};

class Table{
private:
    // Dimensions de la zone de jeu
    float width; 
    float height; 

    // Épaisseur de la bordure
    float borderSize; 

    // Liste des billes sur la table
    std::vector<Boule> balls;

    // Liste des six poches
    std::vector<Pocket> pockets;

    // États du jeu
    Player currentPlayer;
    TableState tableState;
    GameStatus gameStatus;

    // Indique si des billes étaient en mouvement à la frame précédente
    bool isMoving;

    // Enregistrements temporaires du tour courant
    std::vector<BallType> pottedBallsThisTurn; // Billes empochées durant le tour
    bool whiteBallFell;                        // Indicateur de faute : bille blanche empochée

    // Indicateur du mode "bille en main"
    bool placingWhiteBall;

    // Enregistrement des événements d'impact
    bool hasHitAnyBall;         // Indique si la bille blanche a touché une autre bille
    BallType firstHitColor;     // Couleur de la première bille touchée par la blanche

public:
    Table(float w,float h);
    
    // Ajout d'une bille sur la table
    void addBall(const Boule& ball);
    
    // Accesseur en lecture seule des billes
    const std::vector<Boule>& getBalls() const;

    // Accesseur des poches
    const std::vector<Pocket>& getPockets() const;

    // Accesseur de la bille blanche (permet la modification)
    Boule* getWhiteBall();
    
    float getWidth() const{return width; }
    float getHeight() const {return height; }
    float getBorderSize() const {return borderSize; }

    // Mise à jour de la physique et de la logique
    void update(float dt);
    
    // Vérifie si toutes les billes sont immobiles
    bool allBallsStopped() const;

    // Accesseurs d'état
    Player getCurrentPlayer() const { return currentPlayer; }
    GameStatus getGameStatus() const { return gameStatus; }
    
    // Renvoie le nom du joueur
    std::string getPlayerName(Player p) const; 

    // Renvoie l'état de la table sous forme de chaîne de caractères
    std::string getStateString() const;

    // Gestion du mode "bille en main"
    bool isPlacingWhiteBall() const { return placingWhiteBall; }
    
    // Mise à jour de la position de la bille blanche selon le curseur
    void updateWhiteBallPosition(float mouseX, float mouseY);
    
    // Validation du placement de la bille blanche
    bool confirmWhiteBallPlacement();

    // Réinitialisation complète de la partie
    void resetGame();

private:
    // Initialisation des positions des poches
    void initPockets();
    
    // Vérification des empochages
    void checkPockets();

    void switchTurn();
    
    // Traitement de la logique de fin de tour et de l'arbitrage
    void handleTurnLogic(); 
    
    // Activation du mode "bille en main"
    void startPlacementMode();

    // Mise en place du triangle de billes (Rack)
    void setupRack();

    // Vérification de la présence de billes cibles restantes par groupe
    bool hasSolidsLeft() const;
    bool hasStripesLeft() const;
};
