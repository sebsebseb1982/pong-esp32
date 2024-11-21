#ifndef GAME_H
#define GAME_H

#define GAME_WIDTH 58
#define GAME_HEIGHT 64

#define INITIAL_RACKET_SIZE 13
#define MAXIMUM_POINTS 21 // Mutiple de 3
#define RACKET_SPEED_REFRESH_RATE 200
#define MINIMUM_SPEED 16

enum TypeLimit {MIN, MAX};

class Racket {
public:
  Racket(int initialPositionY);
  void loop();
  int positionX;
  int positionY;
  int previousPositionX;
  int previousPositionY;
  int previousSpeedRefreshPositionX;
  unsigned long nextSpeedRefresh;
  float speedX;  // pixels / s
  int size;
};

class Player {
public:
  int score;
  Player(Racket *racket);
  Racket *racket;
};

class Ball {
private:
  unsigned long previousLoopMillis;
  void loopCollisionWalls();
public:
  void loop();
  float speedX;  // pixels / s
  float speedY;  // pixels / s
  float positionX;
  float positionY;
  float previousPositionX;
  float previousPositionY;
  bool hasTouchedWall;
  bool hasTouchedRacket;
  bool isOut;
  void reboundXIfNeeded(float limitX, TypeLimit typeLimit);
  bool reboundYIfNeeded(Racket *racket, TypeLimit typeLimit);
  Ball(void);
};

class Game {
private:
  void loopCollisions();
  void loopScore();
public:
  Ball ball;
  Player *player1;
  Player *player2;
  Game(void);
  void setup();
  void loop();
};

#endif