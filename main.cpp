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
const float COLLISION_MARGIN = 0.2f;

// (1 = wall block, 0 = empty space)
const int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,0,0,1,0,1,1,1,0,1},
    {1,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1},
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
    float x, y;       // player position
    float dirX, dirY; // looking direction
    float planeX, planeY; // camera plane
    float moveSpeed;  // movement speed
    float rotSpeed;   // rotation speed
    float radius;     // ray of the player for collision detection
};


bool checkCollision(float x, float y, float radius) {
    int startX = floor(x - radius);
    int endX = ceil(x + radius);
    int startY = floor(y - radius);
    int endY = ceil(y + radius);
    
    // only check within the bounds of the map
    startX = std::max(0, startX);
    endX = std::min(MAP_WIDTH - 1, endX);
    startY = std::max(0, startY);
    endY = std::min(MAP_HEIGHT - 1, endY);
    
    for (int mapX = startX; mapX <= endX; mapX++) {
        for (int mapY = startY; mapY <= endY; mapY++) {
            if (worldMap[mapX][mapY] > 0) {
                // calculates the distance from the center of the player to the nearest corner of the block
                float closestX = std::max(float(mapX), std::min(x, float(mapX + 1)));
                float closestY = std::max(float(mapY), std::min(y, float(mapY + 1)));
                
                float distX = x - closestX;
                float distY = y - closestY;
                float distSq = distX * distX + distY * distY;
                
                if (distSq < radius * radius) {
                    return true; // collision detected
                }
            }
        }
    }
    return false; // no collision
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Test Raycasting");
    window.setFramerateLimit(60);

    // player initialization
    Player player;
    player.x = 8.0f;
    player.y = 8.0f;
    player.dirX = -1.0f;
    player.dirY = 0.0f;
    player.planeX = 0.0f;
    player.planeY = 0.66f;
    player.moveSpeed = 0.05f;
    player.rotSpeed = 0.03f;
    player.radius = COLLISION_MARGIN; // ray of the player for collision detection

    sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // input handling
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }

        // delta time calculation for fluid motion
        float deltaTime = clock.restart().asSeconds();
        float moveSpeed = player.moveSpeed * deltaTime * 60.0f; // FPS adjustment
        float rotSpeed = player.rotSpeed * deltaTime * 60.0f;   // FPS adjustment

        // forward movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float nextX = player.x + player.dirX * moveSpeed;
            float nextY = player.y + player.dirY * moveSpeed;
            
            // controls collisions separately for X and Y to allow sliding along walls
            if (!checkCollision(nextX, player.y, player.radius)) {
                player.x = nextX;
            }
            if (!checkCollision(player.x, nextY, player.radius)) {
                player.y = nextY;
            }
        }
        // backward movement
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
        // left lateral movement (strafe)
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
        // right lateral movement (strafe)
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
        // left rotation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(rotSpeed) - player.dirY * sin(rotSpeed);
            player.dirY = oldDirX * sin(rotSpeed) + player.dirY * cos(rotSpeed);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(rotSpeed) - player.planeY * sin(rotSpeed);
            player.planeY = oldPlaneX * sin(rotSpeed) + player.planeY * cos(rotSpeed);
        }
        // right rotation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cos(-rotSpeed) - player.dirY * sin(-rotSpeed);
            player.dirY = oldDirX * sin(-rotSpeed) + player.dirY * cos(-rotSpeed);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cos(-rotSpeed) - player.planeY * sin(-rotSpeed);
            player.planeY = oldPlaneX * sin(-rotSpeed) + player.planeY * cos(-rotSpeed);
        }

        window.clear(sf::Color(0, 0, 0));

        // floor and ceiling
        sf::RectangleShape ceiling(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2));
        ceiling.setFillColor(sf::Color(0, 0, 100)); // blue for the ceiling
        ceiling.setPosition(0, 0);
        window.draw(ceiling);

        sf::RectangleShape floor(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2));
        floor.setFillColor(sf::Color(100, 100, 100)); // gray for the floor
        floor.setPosition(0, WINDOW_HEIGHT / 2);
        window.draw(floor);

        // raycast rendering
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            // calculation of the position and direction of the radius
            float cameraX = 2 * x / float(WINDOW_WIDTH) - 1;
            float rayDirX = player.dirX + player.planeX * cameraX;
            float rayDirY = player.dirY + player.planeY * cameraX;

            // initial position
            int mapX = int(player.x);
            int mapY = int(player.y);

            // ray lengths from the current position to the next x or y position
            float sideDistX, sideDistY;

            // lenghts of the ray in the x and y directions
            float deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
            float perpWallDist;

            // step directions
            int stepX, stepY;
            int hit = 0; // flag for wall hit
            int side;    // NS o EW

            // calculation of stepX, stepY and initial lateral distances
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
                // jump to the next square
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                // check if the ray has hit a wall
                if (worldMap[mapX][mapY] > 0) hit = 1;
            }

            // calculation of the distance perpendicular to the wall
            if (side == 0) {
                perpWallDist = (sideDistX - deltaDistX);
            } else {
                perpWallDist = (sideDistY - deltaDistY);
            }

            // calculation of the height of the line to be drawn
            int lineHeight = (int)(WINDOW_HEIGHT / perpWallDist);

            // calculation of the highest and lowest point of the line
            int drawStart = -lineHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawEnd >= WINDOW_HEIGHT) drawEnd = WINDOW_HEIGHT - 1;

            // choice of wall color based on direction
            sf::Color wallColor;
            
            // base color
            switch (worldMap[mapX][mapY]) {
                case 1: wallColor = sf::Color(255, 0, 0); break; // red
                default: wallColor = sf::Color(255, 255, 255); break; // white
            }
            
            // darkening the color based on the side hit N/S or E/W
            if (side == 1) {
                wallColor.r /= 2;
                wallColor.g /= 2;
                wallColor.b /= 2;
            }

            // darker color for fog effect
            float fogFactor = std::min(1.0f, perpWallDist / DEPTH);
            wallColor.r = wallColor.r * (1 - fogFactor) + 0 * fogFactor;
            wallColor.g = wallColor.g * (1 - fogFactor) + 0 * fogFactor;
            wallColor.b = wallColor.b * (1 - fogFactor) + 0 * fogFactor;

            // draw the vertical line for the wall
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(x, drawStart);
            line[0].color = wallColor;
            line[1].position = sf::Vector2f(x, drawEnd);
            line[1].color = wallColor;
            window.draw(line);
        }

        // draw mini-map
        const float mapScale = 10.0f;
        const int mapOffsetX = WINDOW_WIDTH - MAP_WIDTH * mapScale - 10;
        const int mapOffsetY = 10;
        
        // drow the mini-map background
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
        
        // draw the player position on the mini-map
        sf::CircleShape playerDot(mapScale / 3);
        playerDot.setFillColor(sf::Color(0, 255, 0));
        playerDot.setPosition(mapOffsetX + player.x * mapScale - mapScale / 3, 
                             mapOffsetY + player.y * mapScale - mapScale / 3);
        window.draw(playerDot);
        
        // draw the player's direction on the mini-map
        sf::VertexArray direction(sf::Lines, 2);
        direction[0].position = sf::Vector2f(mapOffsetX + player.x * mapScale, 
                                           mapOffsetY + player.y * mapScale);
        direction[0].color = sf::Color::Green;
        direction[1].position = sf::Vector2f(mapOffsetX + (player.x + player.dirX) * mapScale, 
                                           mapOffsetY + (player.y + player.dirY) * mapScale);
        direction[1].color = sf::Color::Green;
        window.draw(direction);

        // HUD - show player position
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