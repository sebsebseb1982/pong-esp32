#ifndef GAME_H
#define GAME_H

#define GAME_WIDTH 64
#define GAME_HEIGHT 64

class Player {
  private:
    int score;
};

class Ball {
  private:
    unsigned long lastLoopMillis;
  public:
    float speedX; // pixels / s
    float speedY; // pixels / s
    float positionX;
    float positionY;
    float lastPositionX;
    float lastPositionY;
    Ball(void);
    void loop();
};

class Game {
  public:
    Ball ball;
    Player player1;
    Player player2;
    Game(void);
    void setup();
    void newGame();
    void loop();
};

#endif