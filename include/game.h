#ifndef GAME_H
#define GAME_H

#define GAME_WIDTH 58
#define GAME_HEIGHT 64

#define INITIAL_RACKET_SIZE 13
#define MAXIMUM_POINTS 21 // Mutiple de 3
#define RACKET_SPEED_REFRESH_RATE 200
#define INITIAL_BALL_SPEED_X  10.0f
#define INITIAL_BALL_SPEED_Y  14.0f
#define BALL_SPEED_ACCEL      1.08f
#define MAX_BALL_SPEED        60.0f
#define INTRO_DURATION_MS 5000
#define GAME_START_DELAY_MS 1500

#define BONUS_DURATION_MS    10000UL
#define BONUS_SPAWN_MIN_MS    5000UL
#define BONUS_SPAWN_MAX_MS   15000UL
#define MAX_FIELD_BONUSES    3
#define MAX_INVENTORY        7
#define ENLARGED_RACKET_SIZE 19
#define SHRUNK_RACKET_SIZE   6
#define MAX_OBSTACLES        4

enum TypeLimit {MIN, MAX};
enum GameState { GAME_INTRO, GAME_STARTING, GAME_PLAYING, GAME_OVER };
enum BonusType { BONUS_SHRINK_ENEMY, BONUS_ENLARGE_SELF, BONUS_OBSTACLE };

struct BonusItem {
  float positionX;
  float positionY;
  BonusType type;
  bool active;
};

struct ObstacleItem {
  int x, y, w, h;
  bool active;
  unsigned long immuneUntil;
};

struct RacketEffect {
  BonusType type;
  unsigned long expiresAt;
  bool active;
};

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
  int previousSize;
  int size;
};

class Player {
public:
  int score;
  Player(Racket *racket);
  Racket *racket;
  BonusType inventory[MAX_INVENTORY];
  int inventoryCount;
};

class Ball {
private:
  unsigned long previousLoopMillis;
  void loopCollisionWalls();
public:
  void loop();
  void reset();
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
  void loopBonusSpawning();
  void loopBonusCollection();
  void loopBonusEffects();
  void loopObstacleCollisions();
public:
  Ball ball;
  Player *player1;
  Player *player2;
  GameState state;
  Player *winner;
  unsigned long introStartMs;
  unsigned long startingStartMs;
  BonusItem fieldBonuses[MAX_FIELD_BONUSES];
  ObstacleItem obstacles[MAX_OBSTACLES];
  Player *lastHitter;
  unsigned long nextBonusSpawnTime;
  RacketEffect racket1Effect;
  RacketEffect racket2Effect;
  Game(void);
  void setup();
  void loop();
  void reset();
  void activateBonus(Player *player);
};

#endif