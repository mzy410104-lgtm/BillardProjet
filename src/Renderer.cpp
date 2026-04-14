#include <SFML/Graphics.hpp>
#include "Renderer.hpp"
#include <iostream>
#include <cmath>
#include "Table.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer(int w,int h) : window(sf::VideoMode(w,h),"Billard"),debugMode(false), isAiming(false){
    window.setFramerateLimit(60);
    
    // Initialisation de l'interface utilisateur (UI)
    initUI();
    // Chargement des ressources graphiques
    loadAssets(); 
}

void Renderer::initUI() {
    // Chargement de la police personnalisée ou système en cas d'échec
    if (!font.loadFromFile("myfont.ttf")) {
        std::cout << "Custom font not found. Loading system Arial..." << std::endl;
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            // Erreur critique si aucune police n'est trouvée
            std::cerr << "CRITICAL ERROR: No fonts found! Text will not display." << std::endl;
        }
    }

    // Configuration du texte du joueur (Haut-gauche)
    playerText.setFont(font);
    playerText.setCharacterSize(30);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(20, 10);

    // Configuration du texte d'état (Haut-droite)
    stateText.setFont(font);
    stateText.setCharacterSize(30);
    stateText.setFillColor(sf::Color::Yellow);
    
    // Configuration du texte de message central (Fautes / Bille en main)
    messageText.setFont(font);
    messageText.setCharacterSize(40);
    messageText.setFillColor(sf::Color::Red);
    messageText.setStyle(sf::Text::Bold);
}

void Renderer::loadAssets() {
    texturesLoaded = true;

    // Chargement des textures avec lissage (anti-aliasing)
    if (!tableTex.loadFromFile("table.png")) { 
        std::cout << "Warning: table.png not found." << std::endl; 
        texturesLoaded = false; 
    } else tableTex.setSmooth(true);

    if (!ballWhiteTex.loadFromFile("ball_white.png")) { texturesLoaded = false; } else ballWhiteTex.setSmooth(true);
    if (!ballBlackTex.loadFromFile("ball_black.png")) { texturesLoaded = false; } else ballBlackTex.setSmooth(true);
    if (!ballRedTex.loadFromFile("ball_red.png"))     { texturesLoaded = false; } else ballRedTex.setSmooth(true);
    if (!ballBlueTex.loadFromFile("ball_blue.png"))   { texturesLoaded = false; } else ballBlueTex.setSmooth(true);

    // Mode sans textures en cas d'échec
    if (!texturesLoaded) {
        std::cout << ">>> Running in Color Mode (Missing Textures) <<<" << std::endl;
    }
}


bool Renderer::isOpen() const{
    return window.isOpen();
}

AppState Renderer::handleInput(Table& table, AppState currentState){
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        // État : Menu principal
        if (currentState == APP_MENU) {
            // Démarrage du jeu avec Entrée
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                // Réinitialisation de la table
                table.resetGame(); 
                return APP_PLAYING;
            }
        }
        
        // État : En jeu (PLAYING)
        else if (currentState == APP_PLAYING) {
            
            // Mise en pause avec Échap
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                return APP_PAUSED; 
            }
        
            // Conversion des coordonnées de la souris en coordonnées physiques
            float tableW = table.getWidth();
            float tableH = table.getHeight();
            float offsetX = (window.getSize().x - tableW) / 2.0f;
            float offsetY = (window.getSize().y - tableH) / 2.0f;
        
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            float mouseX = mousePixel.x - offsetX;
            float mouseY = mousePixel.y - offsetY;
  

            // Mode de placement de la bille blanche (Bille en main)
            if (table.isPlacingWhiteBall()) {
             
                // Mise à jour de la position de la bille blanche
                if (event.type == sf::Event::MouseMoved) {
                    table.updateWhiteBallPosition(mouseX, mouseY);
                }

                // Confirmation du placement avec clic droit
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                    if (table.confirmWhiteBallPlacement()) {
                        // Placement validé
                    } else {
                        // Placement invalide (chevauchement)
                     }
                }
            
                // Désactivation de la frappe pendant le placement
                continue; 
            }

            // Mode de jeu normal (Visée)
            // Activation/Désactivation du mode débogage (Touche D)
            if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::D){
                    toggleDebugMode();
                }
            }
        
            // Début de la visée et enregistrement du point d'impact (Clic gauche)
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                Boule* whiteBall = table.getWhiteBall();
            
                // Vérification : toutes les billes sont arrêtées et la blanche est active
                if (table.allBallsStopped() && whiteBall && whiteBall->isBallActive()) {
                
                    // Calcul des offsets pour la conversion des coordonnées
                    float tableW = table.getWidth();
                    float tableH = table.getHeight();
                    float offsetX = (window.getSize().x - tableW) / 2.0f;
                    float offsetY = (window.getSize().y - tableH) / 2.0f;

                    // Calcul de la position de la bille blanche à l'écran
                    Vector2D ballPos = whiteBall->getPosition();
                    float ballScreenX = ballPos.x + offsetX;
                    float ballScreenY = ballPos.y + offsetY;

                    // Récupération de la position de la souris
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    float dx = mousePos.x - ballScreenX;
                    float dy = mousePos.y - ballScreenY;
                
                    // Calcul de la distance entre la souris et le centre de la bille
                    float dist = std::sqrt(dx*dx + dy*dy);
 
                    // Validation de la visée (clic à l'intérieur de la bille)
                    if (dist <= whiteBall->getRadium()) {
                        isAiming = true;
                    
                        // Normalisation du point d'impact [-1.0, 1.0]
                        hitOffset.x = dx / whiteBall->getRadium();
                        hitOffset.y = dy / whiteBall->getRadium();
                    }
                }
            }
        
            // Exécution du tir (Relâchement du clic gauche)
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (isAiming) {
                    isAiming = false; 
                
                    Boule* whiteBall = table.getWhiteBall();
                    if (whiteBall) {
                        // Recalcul des coordonnées pour le tir
                        float tableW = table.getWidth();
                        float tableH = table.getHeight();
                        float offsetX = (window.getSize().x - tableW) / 2.0f;
                        float offsetY = (window.getSize().y - tableH) / 2.0f;

                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    
                        Vector2D ballPos = whiteBall->getPosition();
                        float ballScreenX = ballPos.x + offsetX;
                        float ballScreenY = ballPos.y + offsetY;

                        float dx = ballScreenX - mousePos.x;
                        float dy = ballScreenY - mousePos.y;
                    
                        float powerFactor = 5.0f; 
                        float length = std::sqrt(dx * dx + dy * dy);
                        if(length > 400.0f){
                            float scale = 400.0f / length;
                            dx *= scale;
                            dy *= scale;
                        }
                    
                        // Calcul de la vélocité finale
                        Vector2D finalVelocity(dx * powerFactor, dy * powerFactor);
                        whiteBall->setVelocity(finalVelocity);

                        // Calcul et application de la rotation (Effets)
                        if (finalVelocity.length() > 1.0f) {
                            Vector2D shotDir = finalVelocity.normalize();
                            
                            // Inversion de l'axe Y pour l'effet de haut/bas
                            float topSpinAmount = -hitOffset.y; 
                            float sideSpinAmount = hitOffset.x;
                            
                            float speed = finalVelocity.length();

                            float spinXYPower = speed * 0.6f; 
                            float spinZPower = speed * 0.008f;

                            Vector2D spinXY = shotDir * (topSpinAmount * spinXYPower);
                            float spinZ = sideSpinAmount * spinZPower;

                            whiteBall->setSpin(spinXY, spinZ);
                        } else {
                            whiteBall->setSpin(Vector2D(0,0), 0.0f);
                        }
                    }
                }
            }
        
        }
        
        // État : En pause (PAUSED)
        else if (currentState == APP_PAUSED) {
            // Reprise du jeu avec Échap
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                return APP_PLAYING; 
            }
            
            // Relance de la partie avec Espace
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                table.resetGame();   
                return APP_PLAYING;  
            }
        }
        
        // État : Fin de partie (GAME_OVER)
        else if (currentState == APP_GAME_OVER) {
            // Relance de la partie depuis l'écran de fin
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                table.resetGame();   
                return APP_PLAYING;  
            }
        }
        
    }
    return currentState;
}

void Renderer::render(const Table& table){
    // Arrière-plan gris foncé
    window.clear(sf::Color(30, 30, 30));
    
    // Centrage de la table
    float tableW = table.getWidth();
    float tableH = table.getHeight();
    float border = table.getBorderSize();
    
    // Calcul des offsets de centrage
    float offsetX = (window.getSize().x - tableW) / 2.0f;
    float offsetY = (window.getSize().y - tableH) / 2.0f;

    // Rendu de la bordure de la table
    sf::RectangleShape borderShape(sf::Vector2f(tableW + border * 2, tableH + border * 2));
    borderShape.setFillColor(sf::Color(101, 67, 33)); 
    borderShape.setPosition(offsetX - border, offsetY - border);
    window.draw(borderShape);

    // Rendu de la surface de jeu
    sf::RectangleShape playArea(sf::Vector2f(tableW, tableH));
    playArea.setPosition(offsetX, offsetY);
    
    // Application de la texture ou de la couleur par défaut
    if (texturesLoaded && tableTex.getSize().x > 0) {
        playArea.setTexture(&tableTex);
        // Couleur de base blanche pour éviter l'altération de la texture
        playArea.setFillColor(sf::Color::White); 
    } else {
        playArea.setFillColor(sf::Color(0, 100, 0)); 
    }
    window.draw(playArea);

    // Rendu des poches
    const auto& pockets = table.getPockets();
    for (const auto& pocket : pockets) {
        sf::CircleShape hole(pocket.radius);
        hole.setFillColor(sf::Color::Black);
        hole.setOrigin(pocket.radius, pocket.radius); 
        hole.setPosition(pocket.position.x + offsetX, pocket.position.y + offsetY);
        window.draw(hole);
    }
    
    // Rendu de la ligne de visée dynamique
    if(isAiming){
        const std::vector<Boule>& balls = table.getBalls();
        const Boule* whiteBall = nullptr;
        for(const auto& b : balls){
            if(b.getType() == WHITE_BALL && b.isBallActive()){
                whiteBall = &b;
                break;
            }
        }
    
        if(whiteBall && whiteBall ->isBallActive()){
            Vector2D ballPos = whiteBall->getPosition();
            float ballScreenX = ballPos.x + offsetX;
            float ballScreenY = ballPos.y + offsetY;
            
            sf::Vector2i MousePos = sf::Mouse::getPosition(window);
            
            // Direction de la queue de billard
            float dx = MousePos.x - ballScreenX;
            float dy = MousePos.y - ballScreenY;
            
            // Angle de rotation de la queue
            float angleRad = std::atan2(dy,dx);
            float angleDeg = angleRad * 180.0f/M_PI;
            
            // Calcul de la distance de recul
            float dist = std::sqrt(dx *dx+dy * dy);
            float pullBack = std::min(dist , 400.0f);
            
            // Configuration visuelle de la queue
            float cueLength = 300.0f;
            float cueWidth = 10.0f;
            sf::RectangleShape cue(sf::Vector2f(cueLength,cueWidth));
            cue.setFillColor(sf::Color(139,69,19));
            
            // Origine de rotation de la queue
            cue.setOrigin(0.0f,cueWidth/2);
            
            // Application de la rotation
            cue.setRotation(angleDeg);
            
            // Distance entre la queue et la bille
            float offsetDist = whiteBall ->getRadium() + pullBack;
            
            if(dist > 0.1f){
                float finalX = ballScreenX +(dx/dist) *offsetDist;
                float finalY = ballScreenY + (dy/dist) *offsetDist;
                cue.setPosition(finalX,finalY);
            }else{
                cue.setPosition(ballScreenX + whiteBall ->getRadium(),ballScreenY);
            }
            window.draw(cue);
            
            // Tracé de la ligne de visée
            sf::Vector2f aimStart(ballScreenX , ballScreenY);
            sf::Vector2f aimEnd(ballScreenX - dx,ballScreenY -dy);
            sf::Vertex aimLine[] = {
                sf::Vertex(aimStart,sf::Color(255,255,255,100)),
                sf::Vertex(aimEnd,sf::Color(255,255,255,0))
            };
            window.draw(aimLine,2,sf::Lines);
        }
    }
    
    // Rendu des billes
    const auto& balls = table.getBalls();
    for (const auto& ball : balls) {
        // Ignorer les billes inactives
        if (!ball.isBallActive()) continue;

        float r = ball.getRadium();
        Vector2D pos = ball.getPosition();

        sf::CircleShape shape(r);
        shape.setOrigin(r, r);
        
        // Application de l'offset de centrage
        shape.setPosition(pos.x + offsetX, pos.y + offsetY);
        
        sf::Texture* targetTex = nullptr;
        sf::Color fallbackColor = sf::Color::White;
        
        shape.setRotation(ball.getRotationAngle());

        // Application de la texture selon le type de bille
        switch(ball.getType()){
            case WHITE_BALL: 
                targetTex = &ballWhiteTex; fallbackColor = sf::Color::White; break;
            case BLACK_BALL: 
                targetTex = &ballBlackTex; fallbackColor = sf::Color::Black; break;
            case RED_BALL:   
                targetTex = &ballRedTex;   fallbackColor = sf::Color::Red;   break;
            case BLUE_BALL:  
                targetTex = &ballBlueTex;  fallbackColor = sf::Color::Blue;  break;
        }

        if (texturesLoaded && targetTex->getSize().x > 0) {
            shape.setTexture(targetTex);
            // Réinitialisation de la couleur pour la texture
            shape.setFillColor(sf::Color::White); 
        } else {
            shape.setTexture(nullptr);
            shape.setFillColor(fallbackColor);
        }
        
        window.draw(shape);

        // Rendu des vecteurs de vitesse (Mode Débogage)
        if (debugMode){
            Vector2D vel = ball.getVelocity();
            float speed = vel.length();

            if (speed > 5.0f){
                sf::Vector2f start(pos.x + offsetX, pos.y + offsetY);
                sf::Vector2f end(pos.x + offsetX + vel.x * 0.3f, pos.y + offsetY + vel.y * 0.3f);
                sf::Vertex line[] = {
                    sf::Vertex(start, sf::Color::Red),
                    sf::Vertex(end, sf::Color::Red)
                };
                window.draw(line, 2, sf::Lines);
            }
        }
    }
    
    // Rendu de l'interface HUD
    
    // Fonction lambda pour le rendu du texte avec fond
    auto drawHUD = [&](sf::Text& text, float padding = 10.0f) {
        // Dimensions du texte
        sf::FloatRect bounds = text.getGlobalBounds();
        
        // Création du rectangle de fond
        sf::RectangleShape bg(sf::Vector2f(bounds.width + padding * 2, bounds.height + padding * 2));
        bg.setFillColor(sf::Color(0, 0, 0, 150)); 
        bg.setPosition(bounds.left - padding, bounds.top - padding); 
        bg.setOutlineColor(sf::Color(200, 200, 200, 100)); 
        bg.setOutlineThickness(1.0f);

        // Affichage du fond puis du texte
        window.draw(bg);
        window.draw(text);
    };

    // Informations du joueur (Haut-gauche)
    std::string playerStr = table.getPlayerName(table.getCurrentPlayer());
    playerText.setString(" Turn: " + playerStr + " "); 
    if (table.getCurrentPlayer() == PLAYER_1) playerText.setFillColor(sf::Color(0, 255, 255)); 
    else playerText.setFillColor(sf::Color(255, 105, 180)); 
    
    // Ajustement de la position
    playerText.setPosition(30, 20); 
    drawHUD(playerText);

    // Informations d'état (Haut-droite)
    std::string stateStr = table.getStateString();
    stateText.setString(" " + stateStr + " ");
    
    // Alignement à droite
    float stateW = stateText.getLocalBounds().width;
    stateText.setPosition(window.getSize().x - stateW - 40, 20);
    drawHUD(stateText);

    // Message spécial (Centré)
    if (table.isPlacingWhiteBall()) {
        messageText.setString(" BALL IN HAND \n Right Click to Place "); 
        // Centrage
        sf::FloatRect msgBounds = messageText.getLocalBounds();
        messageText.setPosition((window.getSize().x - msgBounds.width) / 2.0f, 100);
        
        // Style du fond (Rouge foncé)
        sf::FloatRect bounds = messageText.getGlobalBounds();
        sf::RectangleShape bg(sf::Vector2f(bounds.width + 40, bounds.height + 20));
        bg.setFillColor(sf::Color(100, 0, 0, 200)); 
        bg.setPosition(bounds.left - 20, bounds.top - 10);
        bg.setOutlineColor(sf::Color::Red);
        bg.setOutlineThickness(2.0f);
        
        window.draw(bg);
        window.draw(messageText);
    }
    
    // Rendu du radar d'impact (Effets)
    
    // Configuration du radar (Bas-gauche)
    float radarRadius = 50.0f; 
    sf::Vector2f radarPos(80.0f, window.getSize().y - 80.0f); 

    // Fond du radar
    sf::CircleShape radarBg(radarRadius);
    radarBg.setOrigin(radarRadius, radarRadius); 
    radarBg.setPosition(radarPos);
    radarBg.setFillColor(sf::Color(255, 255, 255, 200)); 
    radarBg.setOutlineThickness(2.0f);
    radarBg.setOutlineColor(sf::Color(100, 100, 100));    
    window.draw(radarBg);

    // Repère en croix (Axial)
    // Ligne horizontale
    sf::RectangleShape hLine(sf::Vector2f(radarRadius * 2, 1.0f));
    hLine.setOrigin(radarRadius, 0.5f);
    hLine.setPosition(radarPos);
    hLine.setFillColor(sf::Color(150, 150, 150, 150));
    window.draw(hLine);
    // Ligne verticale
    sf::RectangleShape vLine(sf::Vector2f(1.0f, radarRadius * 2));
    vLine.setOrigin(0.5f, radarRadius);
    vLine.setPosition(radarPos);
    vLine.setFillColor(sf::Color(150, 150, 150, 150));
    window.draw(vLine);

    // Point d'impact
    float dotRadius = 5.0f;
    sf::CircleShape hitDot(dotRadius);
    hitDot.setOrigin(dotRadius, dotRadius);
    
    // Calcul de la position du point d'impact sur le radar
    float dotX = radarPos.x + hitOffset.x * radarRadius;
    float dotY = radarPos.y + hitOffset.y * radarRadius;
    hitDot.setPosition(dotX, dotY);
    
    // Ajustement de la couleur du point selon l'état de visée
    if (isAiming) {
        hitDot.setFillColor(sf::Color::Red); 
    } else {
        hitDot.setFillColor(sf::Color(255, 0, 0, 80));  
    }
    
    window.draw(hitDot);

    window.display();
}

void Renderer::renderMenu() {
    window.clear(sf::Color(20, 20, 30)); 

    // Titre principal
    sf::Text title("BILLIARD PRO", font, 100);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - titleBounds.width) / 2.0f, window.getSize().y / 3.0f);

    // Message d'invite
    sf::Text prompt("Press [ENTER] to Start", font, 40);
    prompt.setFillColor(sf::Color::Yellow);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setPosition((window.getSize().x - promptBounds.width) / 2.0f, window.getSize().y / 2.0f + 100);

    window.draw(title);
    window.draw(prompt);
    window.display();
}

void Renderer::renderPause(const Table& table) {
    // Arrière-plan foncé
    window.clear(sf::Color(20, 20, 30)); 

    // Texte de pause
    sf::Text title("PAUSED", font, 100);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - titleBounds.width) / 2.0f, window.getSize().y / 3.0f);

    // Invite de reprise
    sf::Text promptResume("Press [ESC] to Resume", font, 40);
    promptResume.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds1 = promptResume.getLocalBounds();
    // Ajustement de la disposition
    promptResume.setPosition((window.getSize().x - promptBounds1.width) / 2.0f, window.getSize().y / 2.0f + 50);

    // Invite de réinitialisation
    sf::Text promptRestart("Press [SPACE] to Restart", font, 40);
    promptRestart.setFillColor(sf::Color(255, 100, 100)); 
    sf::FloatRect promptBounds2 = promptRestart.getLocalBounds();
    promptRestart.setPosition((window.getSize().x - promptBounds2.width) / 2.0f, window.getSize().y / 2.0f + 120);

    // Affichage à l'écran
    window.draw(title);
    window.draw(promptResume);
    window.draw(promptRestart); 
    window.display();
}

void Renderer::renderGameOver(const Table& table) {
    // Arrière-plan sombre
    window.clear(sf::Color(30, 10, 10)); 

    // Récupération de l'état final
    GameStatus status = table.getGameStatus();
    std::string resultStr = "GAME OVER";
    sf::Color resultColor = sf::Color::White;

    // Sélection du texte et de la couleur selon le vainqueur
    if (status == P1_WINS) { resultStr = "PLAYER 1 WINS!"; resultColor = sf::Color::Cyan; }
    else if (status == P2_WINS) { resultStr = "PLAYER 2 WINS!"; resultColor = sf::Color::Magenta; }
    else if (status == P1_LOSE) { resultStr = "P1 FOUL! PLAYER 2 WINS!"; resultColor = sf::Color::Magenta; }
    else if (status == P2_LOSE) { resultStr = "P2 FOUL! PLAYER 1 WINS!"; resultColor = sf::Color::Cyan; }

    // Texte du vainqueur
    sf::Text title(resultStr, font, 80);
    title.setFillColor(resultColor);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - titleBounds.width) / 2.0f, window.getSize().y / 3.0f);

    // Invite de rejeu
    sf::Text prompt("Press [SPACE] to Play Again", font, 40);
    prompt.setFillColor(sf::Color::Yellow);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setPosition((window.getSize().x - promptBounds.width) / 2.0f, window.getSize().y / 2.0f + 100);

    window.draw(title);
    window.draw(prompt);
    window.display();
}