#include "Table.hpp"
#include <cmath>
#include <iostream>


Table::Table(float w,float h) : width(w),height(h), borderSize(50.0f){
    initPockets();
    
    // Initialisation des états du jeu
    currentPlayer = PLAYER_1;   
    tableState = TABLE_OPEN;    
    gameStatus = PLAYING;       
    isMoving = false;           
    hasHitAnyBall = false;
    
    whiteBallFell = false;
    pottedBallsThisTurn.clear();
    
    placingWhiteBall = false;
};

void Table::initPockets(){
    float pocketRadius = 40.0f; 

    // 1. Haut-gauche
    pockets.push_back({Vector2D(0, 0), pocketRadius});
    // 2. Haut-centre
    pockets.push_back({Vector2D(width / 2, 0), pocketRadius});
    // 3. Haut-droite
    pockets.push_back({Vector2D(width, 0), pocketRadius});
    // 4. Bas-gauche
    pockets.push_back({Vector2D(0, height), pocketRadius});
    // 5. Bas-centre
    pockets.push_back({Vector2D(width / 2, height), pocketRadius});
    // 6. Bas-droite
    pockets.push_back({Vector2D(width, height), pocketRadius});
}

Boule* Table::getWhiteBall(){
    for (auto& ball : balls){
        if (ball.getType() == WHITE_BALL){
            return &ball; // Retourne l'adresse de la bille blanche
        }
    }
    return nullptr;
}
        
void Table::addBall(const Boule& ball){
    balls.push_back(ball);
}

const std::vector<Boule>& Table::getBalls() const{
    return balls;
}

const std::vector<Pocket>& Table::getPockets() const{
    return pockets;
}

void Table::checkPockets() {
    for (auto& ball : balls) {
        if (!ball.isBallActive()) continue;
        
        // Ignorer l'empochage de la bille blanche en mode placement
        if (ball.getType() == WHITE_BALL && placingWhiteBall) {
            continue; 
        }

        for (const auto& pocket : pockets) {
            Vector2D delta = ball.getPosition() - pocket.position;
            
            // Vérification de la distance d'empochage
            if (delta.length() < pocket.radius) {
                ball.setInactive(); 
                ball.setVelocity(Vector2D(0,0)); 
                
                // Enregistrement de l'événement
                if (ball.getType() == WHITE_BALL) {
                    whiteBallFell = true;
                    std::cout << ">>> SCRATCH! White Ball Fell!" << std::endl;
                } else {
                    pottedBallsThisTurn.push_back(ball.getType()); 
                    std::cout << ">>> Ball Potted: " << ball.getType() << std::endl;
                }
                
                break; 
            }
        }
    }
}

void Table::update(float dt){
    // Mise à jour de la physique des billes
    for (auto& ball : balls){
        if (ball.isBallActive()){
            ball.update(dt);
            ball.checkWallCollision(width,height);
        }
    }
    
    // Détection des collisions inter-billes
    for (size_t i = 0; i < balls.size(); ++i){
        
        if (!balls[i].isBallActive()) continue;
        
        for (size_t j = i + 1; j < balls.size(); ++j){
            
            if (!balls[j].isBallActive()) continue;
            
            // Mode placement : ignorer les collisions impliquant la bille blanche
            if (placingWhiteBall) {
                Boule* wb = getWhiteBall();
                if (&balls[i] == wb || &balls[j] == wb) {
                    continue; 
                }
            }
            
            // Détection de collision spatiale
            Vector2D delta = balls[i].getPosition() - balls[j].getPosition();
            float dist = delta.length();
            float totalRadius = balls[i].getRadium() + balls[j].getRadium();

            if (dist <= totalRadius) {
                
                // Enregistrement du premier contact de la bille blanche
                if (!hasHitAnyBall) {
                    Boule* wb = getWhiteBall();
                    Boule* other = nullptr;

                    if (&balls[i] == wb) other = &balls[j];
                    else if (&balls[j] == wb) other = &balls[i];

                    if (other != nullptr) {
                        hasHitAnyBall = true;
                        firstHitColor = other->getType();
                    }
                }
            
            balls[i].resolveCollision(balls[j]);
        }
        
    }   
    
    }
    
    // Vérification des empochages
    checkPockets();
    
    // Détection de fin de mouvement et arbitrage
    bool currentlyMoving = !allBallsStopped();
    if (isMoving && !currentlyMoving) {
        handleTurnLogic();
        
        // Réinitialisation des événements pour le tour suivant
        pottedBallsThisTurn.clear();
        whiteBallFell = false;
        hasHitAnyBall = false;
    }
    isMoving = currentlyMoving;
}

bool Table::allBallsStopped() const{
    const float STOP_THRESHOLD = 3.0f;

    for (const auto& ball: balls){
        if(ball.getVelocity().length() > STOP_THRESHOLD){
            return false;
        }
    }
    return true;
}

std::string Table::getPlayerName(Player p) const {
    return (p == PLAYER_1) ? "Player 1" : "Player 2";
}

void Table::switchTurn() {
    if (currentPlayer == PLAYER_1) currentPlayer = PLAYER_2;
    else currentPlayer = PLAYER_1;
    
    std::cout << "Turn Switched! Now it is: " << getPlayerName(currentPlayer) << std::endl;
}



// --- Logique d'Arbitrage ---
void Table::handleTurnLogic() {
    std::cout << "[Referee] Analyzing Shot..." << std::endl;

    // 1. Vérification des conditions de victoire/défaite (Bille noire)
    bool blackPotted = false;
    for (BallType t : pottedBallsThisTurn) {
        if (t == BLACK_BALL) blackPotted = true;
    }

    if (blackPotted) {
        if (whiteBallFell) {
            std::cout << "GAME OVER: Black Potted + Scratch. " << getPlayerName(currentPlayer) << " LOSS!" << std::endl;
            gameStatus = (currentPlayer == PLAYER_1) ? P1_LOSE : P2_LOSE;
        } else {
            std::cout << "GAME OVER: Black Potted. " << getPlayerName(currentPlayer) << " WINS!" << std::endl;
            gameStatus = (currentPlayer == PLAYER_1) ? P1_WINS : P2_WINS;
        }
        
        std::cout << "Starting New Game..." << std::endl;
        
        return; 
    }

  // 2. Détection des fautes
    bool isFoul = false;
    std::string foulReason = "";

    // Cas A : Bille blanche empochée (Scratch)
    if (whiteBallFell) {
        isFoul = true;
        foulReason = "Scratch (White Ball Potted)";
    }
    // Cas B : Tir à vide (Air Shot)
    else if (!hasHitAnyBall) {
        isFoul = true;
        foulReason = "Air Shot (Hit Nothing)";
    }
    // Cas C : Mauvaise bille touchée en premier
    else {
        BallType targetType;
        
        if (tableState == TABLE_OPEN) {
            targetType = BLACK_BALL; 
        } 
        else {
            if (tableState == P1_RED_P2_BLUE) {
                targetType = (currentPlayer == PLAYER_1) ? RED_BALL : BLUE_BALL;
            } else {
                targetType = (currentPlayer == PLAYER_1) ? BLUE_BALL : RED_BALL;
            }
        }

        // Évaluation du premier contact
        if (tableState == TABLE_OPEN) {
            if (firstHitColor == BLACK_BALL) {
                isFoul = true;
                foulReason = "Hit Black First on Open Table";
            }
        } 
        else {
            bool amIRed = (targetType == RED_BALL);
            bool ballsLeft = amIRed ? hasSolidsLeft() : hasStripesLeft();

            if (ballsLeft) {
                if (firstHitColor != targetType) {
                    isFoul = true;
                    foulReason = "Wrong Ball First";
                }
            } else {
                if (firstHitColor != BLACK_BALL) {
                    isFoul = true;
                    foulReason = "Did not hit Black First";
                }
            }
        }
    }

    // Application des pénalités
    if (isFoul) {
        std::cout << "FOUL! " << foulReason << std::endl;
        
        // Activation de la bille en main
        startPlacementMode();
        switchTurn();
        return;
    }

    // 3. Gestion des empochages et attribution des groupes
    if (pottedBallsThisTurn.empty()) {
        std::cout << "No balls potted. Switch turn." << std::endl;
        switchTurn();
        return;
    }

    BallType firstPotted = pottedBallsThisTurn[0]; 

    // Table ouverte
    if (tableState == TABLE_OPEN) {
        if (firstPotted == RED_BALL) {
            tableState = (currentPlayer == PLAYER_1) ? P1_RED_P2_BLUE : P1_BLUE_P2_RED;
            std::cout << "Groups Assigned! " << getPlayerName(currentPlayer) << " is RED." << std::endl;
        } else if (firstPotted == BLUE_BALL) {
            tableState = (currentPlayer == PLAYER_1) ? P1_BLUE_P2_RED : P1_RED_P2_BLUE;
            std::cout << "Groups Assigned! " << getPlayerName(currentPlayer) << " is BLUE." << std::endl;
        }
        std::cout << "Continue Turn." << std::endl;
        return;
    }

    // Table fermée (Groupes attribués)
    BallType myColor;
    if (tableState == P1_RED_P2_BLUE) myColor = (currentPlayer == PLAYER_1) ? RED_BALL : BLUE_BALL;
    else myColor = (currentPlayer == PLAYER_1) ? BLUE_BALL : RED_BALL;

    bool pottedMyBall = false;
    for (BallType t : pottedBallsThisTurn) {
        if (t == myColor) pottedMyBall = true;
    }

    if (pottedMyBall) {
        std::cout << "Good shot! Potted own ball. Continue." << std::endl;
    } else {
        std::cout << "Potted opponent's ball only. Switch turn." << std::endl;
        switchTurn();
    }
}

// Fonction de formatage pour l'interface
std::string Table::getStateString() const {
    if (tableState == TABLE_OPEN) return "Open Table";
    if (tableState == P1_RED_P2_BLUE) return "P1: Red | P2: Blue";
    return "P1: Blue | P2: Red";
}

void Table::startPlacementMode() {
    placingWhiteBall = true;
    
    // Réapparition de la bille blanche au centre
    Boule* wb = getWhiteBall();
    if (wb) {
        wb->revive();
        wb->setVelocity(Vector2D(0,0));
        wb->setPosition(Vector2D(width/2, height/2));
    }
}

void Table::updateWhiteBallPosition(float mouseX, float mouseY) {
    if (!placingWhiteBall) return;
    
    Boule* wb = getWhiteBall();
    if (wb) {
        // Limitation aux dimensions de la table
        float r = wb->getRadium();
        if (mouseX < r) mouseX = r;
        if (mouseX > width - r) mouseX = width - r;
        if (mouseY < r) mouseY = r;
        if (mouseY > height - r) mouseY = height - r;
        
        wb->setPosition(Vector2D(mouseX, mouseY));
    }
}

bool Table::confirmWhiteBallPlacement() {
    if (!placingWhiteBall) return false;

    Boule* wb = getWhiteBall();
    if (!wb) return false;

    // Vérification du chevauchement
    Vector2D myPos = wb->getPosition();
    float myR = wb->getRadium();

    for (const auto& other : balls) {
        if (&other == wb || !other.isBallActive()) continue;

        float dist = (myPos - other.getPosition()).length();
        if (dist < (myR + other.getRadium())) {
            std::cout << "Placement Invalid: Overlapping!" << std::endl;
            return false; 
        }
    }

    // Validation du placement
    placingWhiteBall = false; 
    std::cout << "White Ball Placed." << std::endl;
    return true;
}

// Réinitialisation complète de la partie
void Table::resetGame() {
    // 1. Suppression des billes
    balls.clear();

    // 2. Réinitialisation des états
    currentPlayer = PLAYER_1;
    tableState = TABLE_OPEN;
    gameStatus = PLAYING;
    isMoving = false;
    whiteBallFell = false;
    placingWhiteBall = false;
    pottedBallsThisTurn.clear();
    hasHitAnyBall = false;
    
    // 3. Mise en place du triangle
    setupRack();

    std::cout << "=== GAME RESET ===" << std::endl;
}

// Création de la formation initiale (Rack)
void Table::setupRack() {
    float r = 20.0f; 
    float m = 1.0f;  

    // 1. Placement de la bille blanche
    float whiteX = width * 0.25f; 
    float whiteY = height * 0.5f; 
    Boule whiteBall(Vector2D(whiteX, whiteY), r, m, WHITE_BALL);
    addBall(whiteBall);

    // 2. Placement des 15 billes cibles
    float startX = width * 0.75f; 
    float startY = height * 0.5f; 
    
    float rowSpacing = r * 1.732f;
    float colSpacing = r * 2.0f + 1.0f;

    int racking[5][5] = {
        {0},                   
        {1, 0},                
        {1, 2, 0},             
        {0, 1, 0, 1},          
        {1, 0, 1, 0, 1}        
    };
    
    int colCounts[] = {1, 2, 3, 4, 5}; 

    for (int i = 0; i < 5; ++i) {
        float x = startX + i * rowSpacing;
        float currentColumnHeight = (colCounts[i] - 1) * colSpacing;
        float topY = startY - currentColumnHeight / 2.0f;

        for (int j = 0; j < colCounts[i]; ++j) {
            float y = topY + j * colSpacing;
            
            BallType type;
            int code = racking[i][j];
            
            if (code == 0) type = RED_BALL;
            else if (code == 1) type = BLUE_BALL;
            else type = BLACK_BALL;

            Boule objBall(Vector2D(x, y), r, m, type);
            addBall(objBall);
        }
    }
}


// Vérification de la présence de billes rouges/bleues
bool Table::hasSolidsLeft() const {
    for (const auto& ball : balls) {
        if (ball.isBallActive() && ball.getType() == RED_BALL) return true;
    }
    return false;
}

bool Table::hasStripesLeft() const {
    for (const auto& ball : balls) {
        if (ball.isBallActive() && ball.getType() == BLUE_BALL) return true;
    }
    return false;
}