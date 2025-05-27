#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;
const float FOV = 60.0f * M_PI / 180.0f;
const float DEPTH = 16.0f;
const float COLLISION_MARGIN = 0.2f; // Margine di collisione per impedire di avvicinarsi troppo ai muri

// Definizione semplificata della mappa (1 = muro, 0 = spazio vuoto)
const int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,1},
    {1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1},
    {1,0,0,1,1,1,0,0,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,1},
    {1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,1,0,0,0,1,1,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct Player {
    float x, y;       // Posizione del giocatore
    float dirX, dirY; // Direzione di osservazione
    float planeX, planeY; // Piano della camera
    float moveSpeed;  // Velocità di movimento
    float rotSpeed;   // Velocità di rotazione
    float radius;     // Raggio del giocatore per le collisioni
};

// Funzione per verificare le collisioni con i muri
bool checkCollision(float x, float y, float radius) {
    // Controlla tutti i blocchi che potrebbero sovrapporsi con il cerchio del giocatore
    int startX = floor(x - radius);
    int endX = ceil(x + radius);
    int startY = floor(y - radius);
    int endY = ceil(y + radius);
    
    // Assicuriamoci che stiamo controllando solo blocchi all'interno della mappa
    startX = std::max(0, startX);
    endX = std::min(MAP_WIDTH - 1, endX);
    startY = std::max(0, startY);
    endY = std::min(MAP_HEIGHT - 1, endY);
    
    for (int mapX = startX; mapX <= endX; mapX++) {
        for (int mapY = startY; mapY <= endY; mapY++) {
            if (worldMap[mapX][mapY] > 0) {
                // Calcola la distanza dal centro del giocatore all'angolo più vicino del blocco
                float closestX = std::max(float(mapX), std::min(x, float(mapX + 1)));
                float closestY = std::max(float(mapY), std::min(y, float(mapY + 1)));
                
                float distX = x - closestX;
                float distY = y - closestY;
                float distSq = distX * distX + distY * distY;
                
                if (distSq < radius * radius) {
                    return true; // Collisione rilevata
                }
            }
        }
    }
    return false; // Nessuna collisione
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Raycaster - Stile Doom");
    window.setFramerateLimit(60);

    // Inizializzazione del giocatore
    Player player;
    player.x = 8.0f;
    player.y = 8.0f;
    player.dirX = -1.0f;
    player.dirY = 0.0f;
    player.planeX = 0.0f;
    player.planeY = 0.66f;
    player.moveSpeed = 0.05f;
    player.rotSpeed = 0.03f;
    player.radius = COLLISION_MARGIN; // Raggio del giocatore per le collisioni

    sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Gestione dell'input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }

        // Calcolo delta time per movimento fluido
        float deltaTime = clock.restart().asSeconds();
        float moveSpeed = player.moveSpeed * deltaTime * 60.0f; // Aggiustamento per FPS
        float rotSpeed = player.rotSpeed * deltaTime * 60.0f;   // Aggiustamento per FPS

        // Movimento avanti
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float nextX = player.x + player.dirX * moveSpeed;
            float nextY = player.y + player.dirY * moveSpeed;
            
            // Controlla le collisioni separatamente per X e Y per permettere lo scivolamento lungo i muri
            if (!checkCollision(nextX, player.y, player.radius)) {
                player.x = nextX;
            }
            if (!checkCollision(player.x, nextY, player.radius)) {
                player.y = nextY;
            }
        }
        // Movimento indietro
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            float nextX = player.x - player.dirX * moveSpeed;
            float nextY = player.y - player.dirY * moveSpeed;
            
            if (!checkCollision(nextX, player.y, player.radius)) {
                player.x = nextX;
            }
            if (!checkCollision(player.x, nextY, player.radius)) {
                player.y = nextY;
            }
        }
        // Movimento laterale sinistra (strafe)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            float nextX = player.x - player.planeX * moveSpeed;
            float nextY = player.y - player.planeY * moveSpeed;
            
            if (!checkCollision(nextX, player.y, player.radius)) {
                player.x = nextX;
            }
            if (!checkCollision(player.x, nextY, player.radius)) {
                player.y = nextY;
            }
        }
        // Movimento laterale destra (strafe)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            float nextX = player.x + player.planeX * moveSpeed;
            float nextY = player.y + player.planeY * moveSpeed;
            
            if (!checkCollision(nextX, player.y, player.radius)) {
                player.x = nextX;
            }
            if (!checkCollision(player.x, nextY, player.radius)) {
                player.y = nextY;
            }
        }
        // Rotazione a sinistra
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rotSpeed) - player.dirY * sin(rotSpeed);
            player.dirY = oldDirX * sin(rotSpeed) + player.dirY * cos(rotSpeed);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rotSpeed) - player.planeY * sin(rotSpeed);
            player.planeY = oldPlaneX * sin(rotSpeed) + player.planeY * cos(rotSpeed);
        }
        // Rotazione a destra
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(-rotSpeed) - player.dirY * sin(-rotSpeed);
            player.dirY = oldDirX * sin(-rotSpeed) + player.dirY * cos(-rotSpeed);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(-rotSpeed) - player.planeY * sin(-rotSpeed);
            player.planeY = oldPlaneX * sin(-rotSpeed) + player.planeY * cos(-rotSpeed);
        }

        window.clear(sf::Color(0, 0, 0));

        // Floor e ceiling
        sf::RectangleShape ceiling(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2));
        ceiling.setFillColor(sf::Color(0, 0, 100)); // Blu scuro per il cielo
        ceiling.setPosition(0, 0);
        window.draw(ceiling);

        sf::RectangleShape floor(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2));
        floor.setFillColor(sf::Color(100, 100, 100)); // Grigio per il pavimento
        floor.setPosition(0, WINDOW_HEIGHT / 2);
        window.draw(floor);

        // Raycast rendering
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            // Calcolo della posizione e direzione del raggio
            float cameraX = 2 * x / float(WINDOW_WIDTH) - 1;
            float rayDirX = player.dirX + player.planeX * cameraX;
            float rayDirY = player.dirY + player.planeY * cameraX;

            // Posizione iniziale
            int mapX = int(player.x);
            int mapY = int(player.y);

            // Lunghezze del raggio dall'attuale posizione alla prossima x o y
            float sideDistX, sideDistY;

            // Lunghezza del raggio da una x o y alla successiva
            float deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
            float perpWallDist;

            // Direzione di step
            int stepX, stepY;
            int hit = 0; // Flag per indicare se è stato colpito un muro
            int side;    // NS o EW

            // Calcolo di stepX, stepY e delle distanze laterali iniziali
            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (player.x - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0 - player.x) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (player.y - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - player.y) * deltaDistY;
            }

            // DDA (Digital Differential Analysis)
            while (hit == 0) {
                // Salto al prossimo quadrato della mappa
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                // Controllo se è stato colpito un muro
                if (worldMap[mapX][mapY] > 0) hit = 1;
            }

            // Calcolo della distanza perpendicolare alla parete
            if (side == 0) {
                perpWallDist = (sideDistX - deltaDistX);
            } else {
                perpWallDist = (sideDistY - deltaDistY);
            }

            // Calcolo dell'altezza della linea da disegnare
            int lineHeight = (int)(WINDOW_HEIGHT / perpWallDist);

            // Calcolo del punto più alto e più basso della linea
            int drawStart = -lineHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawEnd >= WINDOW_HEIGHT) drawEnd = WINDOW_HEIGHT - 1;

            // Scelta del colore della parete in base alla direzione
            sf::Color wallColor;
            
            // Colore base
            switch (worldMap[mapX][mapY]) {
                case 1: wallColor = sf::Color(255, 0, 0); break; // Rosso
                default: wallColor = sf::Color(255, 255, 255); break; // Bianco
            }
            
            // Rendi il colore più scuro per le pareti N/S
            if (side == 1) {
                wallColor.r /= 2;
                wallColor.g /= 2;
                wallColor.b /= 2;
            }

            // Rendi il colore più scuro in base alla distanza (nebbia)
            float fogFactor = std::min(1.0f, perpWallDist / DEPTH);
            wallColor.r = wallColor.r * (1 - fogFactor) + 0 * fogFactor;
            wallColor.g = wallColor.g * (1 - fogFactor) + 0 * fogFactor;
            wallColor.b = wallColor.b * (1 - fogFactor) + 0 * fogFactor;

            // Disegna la linea verticale
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(x, drawStart);
            line[0].color = wallColor;
            line[1].position = sf::Vector2f(x, drawEnd);
            line[1].color = wallColor;
            window.draw(line);
        }

        // Disegna la mini-mappa
        const float mapScale = 10.0f;
        const int mapOffsetX = WINDOW_WIDTH - MAP_WIDTH * mapScale - 10;
        const int mapOffsetY = 10;
        
        // Disegna i blocchi della mappa
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (worldMap[x][y] > 0) {
                    sf::RectangleShape block(sf::Vector2f(mapScale, mapScale));
                    block.setPosition(mapOffsetX + x * mapScale, mapOffsetY + y * mapScale);
                    block.setFillColor(sf::Color(255, 0, 0));
                    window.draw(block);
                } else {
                    sf::RectangleShape block(sf::Vector2f(mapScale, mapScale));
                    block.setPosition(mapOffsetX + x * mapScale, mapOffsetY + y * mapScale);
                    block.setFillColor(sf::Color(50, 50, 50));
                    block.setOutlineColor(sf::Color(100, 100, 100));
                    block.setOutlineThickness(1);
                    window.draw(block);
                }
            }
        }
        
        // Disegna il giocatore sulla mini-mappa
        sf::CircleShape playerDot(mapScale / 3);
        playerDot.setFillColor(sf::Color(0, 255, 0));
        playerDot.setPosition(mapOffsetX + player.x * mapScale - mapScale / 3, 
                             mapOffsetY + player.y * mapScale - mapScale / 3);
        window.draw(playerDot);
        
        // Disegna la direzione del giocatore
        sf::VertexArray direction(sf::Lines, 2);
        direction[0].position = sf::Vector2f(mapOffsetX + player.x * mapScale, 
                                           mapOffsetY + player.y * mapScale);
        direction[0].color = sf::Color::Green;
        direction[1].position = sf::Vector2f(mapOffsetX + (player.x + player.dirX) * mapScale, 
                                           mapOffsetY + (player.y + player.dirY) * mapScale);
        direction[1].color = sf::Color::Green;
        window.draw(direction);

        // HUD - mostra la posizione e la direzione del giocatore
        sf::Text positionText;
        sf::Font font;
        if (font.loadFromFile("arial.ttf")) {
            positionText.setFont(font);
            positionText.setCharacterSize(20);
            positionText.setFillColor(sf::Color::White);
            positionText.setPosition(10, 10);
            positionText.setString("Pos: (" + std::to_string(player.x).substr(0, 4) + "," + 
                                  std::to_string(player.y).substr(0, 4) + ")");
            window.draw(positionText);
        }

        window.display();
    }

    return 0;
}