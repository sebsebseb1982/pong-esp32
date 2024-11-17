#ifndef GAME_H
#define GAME_H

#define GAME_WIDTH 58
#define GAME_HEIGHT 64

#define INITIAL_RACKET_SIZE 6
#define MAXIMUM_POINTS 21 // Mutiple de 3

enum TypeLimit {MIN, MAX};

class Racket {
public:
  Racket(int initialPositionY);
  int positionX;
  int positionY;
  int lastPositionX;
  int lastPositionY;
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
  unsigned long lastLoopMillis;
  void loopCollisionWalls();
public:
  float speedX;  // pixels / s
  float speedY;  // pixels / s
  float positionX;
  float positionY;
  float lastPositionX;
  float lastPositionY;
  bool hasTouchedWall;
  bool hasTouchedRacket;
  bool isOut;
  void reboundXIfNeeded(float limitX, TypeLimit typeLimit);
  bool reboundYIfNeeded(Racket *racket, TypeLimit typeLimit);
  Ball(void);
  void loop();
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