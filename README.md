🎱 Jeu de Billard 2D - Projet C++ (SFML)Bienvenue dans notre projet de simulation de billard 2D ! Ce projet a été réalisé en C++ avec la bibliothèque SFML dans le cadre de notre cursus universitaire. 

📝 DescriptionCe logiciel est une simulation interactive de billard (règles du 8-Ball américain) intégrant un moteur physique réaliste, une gestion complète des tours et une interface utilisateur intuitive. 

🚀 Fonctionnalités PrincipalesMode Deux Joueurs : Gestion complète du cycle de jeu et passage fluide entre les joueurs. Moteur Physique Robuste :Intégration d'Euler et Sub-stepping (4 micro-itérations par frame) pour une stabilité numérique optimale. Gestion des collisions élastiques entre les billes et rebonds sur les bandes. Modèle de friction linéaire pour un arrêt naturel des billes. Gestion des effets : Spin latéral (Side-spin) et glissement (Rétro/Coulé). Système d'Arbitrage (Referee) :Attribution dynamique des groupes (Rouge vs Bleu). Détection automatique des fautes (Scratch, Air Shot, Wrong Ball). Mécanisme de Bille en main avec vérification de non-chevauchement. Interface Graphique (HUD) :Affichage en temps réel du joueur actuel et de l'état de la table. Radar d'impact : Visualisation précise du point de contact sur la bille blanche. Ligne de visée dynamique et gestion de la puissance de tir. 

🛠️ Architecture du ProjetLe projet est structuré en plusieurs modules orientés objet : main : Point d'entrée, boucle principale et gestion des états (Menu, Play, Pause, GameOver). Renderer : Pipeline graphique SFML, gestion des entrées et HUD. Table : Cerveau du jeu, gère l'arbitrage et les composants de la table. Boule : Entité physique gérant le mouvement, les collisions et la rotation. Vector2D : Bibliothèque mathématique pour les calculs vectoriels. 

🎮 Commandes (Contrôles)Souris (Clic Gauche) : Cliquer sur la bille blanche pour viser. Tirer vers l'arrière pour ajuster la puissance, puis relâcher pour tirer.Souris (Clic Droit) : En mode "Bille en main", confirmer le placement de la blanche.Touche \[ENTER] : Commencer le jeu depuis le menu.Touche \[ESC] : Mettre le jeu en pause / Reprendre.Touche \[SPACE] : Recommencer une partie (en mode Pause ou GameOver).

📦 InstallationAssurez-vous d'avoir installé la bibliothèque SFML 2.5+.Clonez le dépôt :Bashgit clone https://github.com/mzy410104-lgtm/BillardProjet.git

Compilez le projet en incluant les dossiers SFML.Placez les ressources (table.png, ball\_white.png, etc.) et le fichier myfont.ttf dans le répertoire de l'exécutable.

👥 ContributeursZhaoyiYanYihan

