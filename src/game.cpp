#include <Arduino.h>
#include "game.h"
#include "buzzer.h"

Game::Game(void) {
  this->player1 = new Player(new Racket(0));
  this->player2 = new Player(new Racket(GAME_HEIGHT - 1));
  this->state = GAME_INTRO;
  this->winner = nullptr;
  this->introStartMs = 0;
  this->startingStartMs = 0;
  this->lastHitter = nullptr;
  this->nextBonusSpawnTime = 0;
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    this->fieldBonuses[i].active = false;
  }
  this->racket1Effect.active = false;
  this->racket2Effect.active = false;
}

void Game::setup() {
  randomSeed(micros());
  this->introStartMs = millis();
  this->nextBonusSpawnTime = millis() + random(BONUS_SPAWN_MIN_MS, BONUS_SPAWN_MAX_MS + 1);
}

void Game::loopCollisions() {
  // Walls
  this->ball.reboundXIfNeeded(0.0, MIN);
  this->ball.reboundXIfNeeded((GAME_WIDTH - 1) * 1.0, MAX);

  // Rackets – track last hitter for bonus collection
  this->ball.hasTouchedRacket = false;
  bool p1miss = this->ball.reboundYIfNeeded(this->player1->racket, MIN);
  if (this->ball.hasTouchedRacket) this->lastHitter = this->player1;
  this->player2->score += p1miss ? 1 : 0;

  bool p1hit = this->ball.hasTouchedRacket;
  this->ball.hasTouchedRacket = false;
  bool p2miss = this->ball.reboundYIfNeeded(this->player2->racket, MAX);
  if (this->ball.hasTouchedRacket) this->lastHitter = this->player2;
  this->ball.hasTouchedRacket = this->ball.hasTouchedRacket || p1hit;
  this->player1->score += p2miss ? 1 : 0;
}

void Game::loopBonusSpawning() {
  if (millis() < this->nextBonusSpawnTime) return;

  int freeSlot = -1;
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    if (!this->fieldBonuses[i].active) { freeSlot = i; break; }
  }
  if (freeSlot != -1) {
    this->fieldBonuses[freeSlot].positionX = random(GAME_WIDTH / 4, GAME_WIDTH * 3 / 4 + 1);
    this->fieldBonuses[freeSlot].positionY = random(GAME_HEIGHT / 4, GAME_HEIGHT * 3 / 4 + 1);
    this->fieldBonuses[freeSlot].type = (random(2) == 0) ? BONUS_SHRINK_ENEMY : BONUS_ENLARGE_SELF;
    this->fieldBonuses[freeSlot].active = true;
  }
  this->nextBonusSpawnTime = millis() + random(BONUS_SPAWN_MIN_MS, BONUS_SPAWN_MAX_MS + 1);
}

void Game::loopBonusCollection() {
  if (this->lastHitter == nullptr) return;

  int ballX = (int)this->ball.positionX;
  int ballY = (int)this->ball.positionY;
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    if (!this->fieldBonuses[i].active) continue;
    if ((int)this->fieldBonuses[i].positionX == ballX &&
        (int)this->fieldBonuses[i].positionY == ballY) {
      if (this->lastHitter->inventoryCount < MAX_INVENTORY) {
        this->lastHitter->inventory[this->lastHitter->inventoryCount] = this->fieldBonuses[i].type;
        this->lastHitter->inventoryCount++;
      }
      this->fieldBonuses[i].active = false;
    }
  }
}

void Game::loopBonusEffects() {
  unsigned long now = millis();
  if (this->racket1Effect.active && now >= this->racket1Effect.expiresAt) {
    this->player1->racket->size = INITIAL_RACKET_SIZE;
    this->racket1Effect.active = false;
  }
  if (this->racket2Effect.active && now >= this->racket2Effect.expiresAt) {
    this->player2->racket->size = INITIAL_RACKET_SIZE;
    this->racket2Effect.active = false;
  }
}

void Game::activateBonus(Player *player) {
  if (player->inventoryCount <= 0) return;
  if (this->state != GAME_PLAYING) return;

  BonusType bonusType = player->inventory[player->inventoryCount - 1];
  player->inventoryCount--;

  RacketEffect *effect;
  Racket *targetRacket;
  int newSize;

  if (bonusType == BONUS_SHRINK_ENEMY) {
    targetRacket = (player == this->player1) ? this->player2->racket : this->player1->racket;
    effect       = (player == this->player1) ? &this->racket2Effect  : &this->racket1Effect;
    newSize = SHRUNK_RACKET_SIZE;
  } else {
    targetRacket = player->racket;
    effect       = (player == this->player1) ? &this->racket1Effect  : &this->racket2Effect;
    newSize = ENLARGED_RACKET_SIZE;
  }

  targetRacket->size = newSize;
  effect->type      = bonusType;
  effect->expiresAt = millis() + BONUS_DURATION_MS;
  effect->active    = true;
}

void Game::loopScore() {
  if (this->player1->score >= MAXIMUM_POINTS) {
    this->winner = this->player1;
    this->state = GAME_OVER;
  } else if (this->player2->score >= MAXIMUM_POINTS) {
    this->winner = this->player2;
    this->state = GAME_OVER;
  }
}

void Game::loop() {
  if (this->state == GAME_INTRO) {
    if (millis() - this->introStartMs >= INTRO_DURATION_MS) {
      this->ball.reset();
      this->state = GAME_STARTING;
      this->startingStartMs = millis();
    }
    this->player1->racket->loop();
    this->player2->racket->loop();
    return;
  }
  if (this->state == GAME_STARTING) {
    if (millis() - this->startingStartMs >= GAME_START_DELAY_MS) {
      this->ball.reset();
      this->state = GAME_PLAYING;
    }
    this->player1->racket->loop();
    this->player2->racket->loop();
    return;
  }
  if (this->state == GAME_OVER) {
    return;
  }
  // GAME_PLAYING
  this->player1->racket->loop();
  this->player2->racket->loop();
  this->ball.loop();
  this->loopCollisions();
  this->loopBonusSpawning();
  this->loopBonusCollection();
  this->loopBonusEffects();
  this->loopScore();
}

Racket::Racket(int positionY) {
  this->positionX = 0;
  this->positionY = positionY;
  this->previousPositionX = -1;
  this->previousPositionY = -1;
  this->nextSpeedRefresh = millis() + RACKET_SPEED_REFRESH_RATE;
  this->speedX = 0.0;
  this->size = INITIAL_RACKET_SIZE;
  this->previousSize = -1;
}

void Racket::loop() {
  if (millis() >= this->nextSpeedRefresh) {
    this->speedX = (this->positionX - this->previousSpeedRefreshPositionX) / (RACKET_SPEED_REFRESH_RATE / 1000.0);
    this->previousSpeedRefreshPositionX = this->positionX;
    this->nextSpeedRefresh = millis() + RACKET_SPEED_REFRESH_RATE;
  }
}

Player::Player(Racket *racket) {
  this->score = 0;
  this->racket = racket;
  this->inventoryCount = 0;
}

Ball::Ball(void) {
  this->positionX = 1.0;
  this->positionY = 1.0;
  this->previousPositionX = this->positionX;
  this->previousPositionY = this->positionY;
  this->speedX = INITIAL_BALL_SPEED_X;
  this->speedY = INITIAL_BALL_SPEED_Y;
  this->previousLoopMillis = millis();
  this->hasTouchedWall = false;
  this->hasTouchedRacket = false;
  this->isOut = false;
}

void Ball::reboundXIfNeeded(float limitX, TypeLimit typeLimit) {
  bool needToRebound = false;
  if (this->speedX < 0.0 && this->positionX <= limitX && typeLimit == MIN) {
    this->hasTouchedWall = true;
    this->positionX = limitX + (limitX - this->positionX);
    needToRebound = true;
  } else if (this->speedX > 0.0 && this->positionX >= limitX && typeLimit == MAX) {
    this->hasTouchedWall = true;
    this->positionX = limitX - (this->positionX - limitX);
    needToRebound = true;
  }

  if (needToRebound) {
    this->speedX = this->speedX * -BALL_SPEED_ACCEL;
    if (this->speedX > MAX_BALL_SPEED) this->speedX = MAX_BALL_SPEED;
    if (this->speedX < -MAX_BALL_SPEED) this->speedX = -MAX_BALL_SPEED;
  }
}

bool Ball::reboundYIfNeeded(Racket *racket, TypeLimit typeLimit) {
  bool missedBall = false;
  bool needToRebound = false;
  float limitY = typeLimit == MIN ? (racket->positionY + 1) * 1.0 : (racket->positionY - 1) * 1.0;
  float racketMinX = racket->positionX * 1.0;
  float racketMaxX = (racket->positionX + racket->size) * 1.0;
  if (this->speedY < 0.0 && this->positionY <= limitY && typeLimit == MIN) {
    if (racketMinX <= this->positionX && this->positionX <= racketMaxX) {
      this->hasTouchedRacket = true;
      this->speedX += racket->speedX * 0.6;
      this->speedY += racket->speedX * 0.3;
    } else {
      this->isOut = true;
      missedBall = true;
    }
    needToRebound = true;
    this->positionY = limitY + (limitY - this->positionY);
  } else if (this->speedY > 0.0 && this->positionY >= limitY && typeLimit == MAX) {
    if (racketMinX <= this->positionX && this->positionX <= racketMaxX) {
      this->hasTouchedRacket = true;
      this->speedX += racket->speedX * 0.6;
      this->speedY += racket->speedX * 0.3;
    } else {
      this->isOut = true;
      missedBall = true;
    }
    needToRebound = true;
    this->positionY = limitY - (this->positionY - limitY);
  }

  if (needToRebound) {
    if (missedBall) {
      // Point terminé – réinitialiser la vitesse au départ lent
      this->speedX = (this->speedX >= 0) ? INITIAL_BALL_SPEED_X : -INITIAL_BALL_SPEED_X;
      this->speedY = (this->speedY < 0)  ? INITIAL_BALL_SPEED_Y : -INITIAL_BALL_SPEED_Y;
    } else {
      // Touche de raquette – accélérer progressivement
      this->speedY = this->speedY * -BALL_SPEED_ACCEL;
      if (this->speedY > MAX_BALL_SPEED) this->speedY = MAX_BALL_SPEED;
      if (this->speedY < -MAX_BALL_SPEED) this->speedY = -MAX_BALL_SPEED;
    }
  }
  return missedBall;
}

void Ball::loop() {
  Serial.println("Ball::loop()");
  unsigned long nowMillis = millis();
  this->hasTouchedWall = false;
  this->hasTouchedRacket = false;
  this->isOut = false;
  this->previousPositionX = this->positionX;
  this->previousPositionY = this->positionY;

  float deltaLoopInSeconds = (nowMillis * 1.0 - this->previousLoopMillis * 1.0) / 1000.0;
  this->positionX = this->positionX + this->speedX * deltaLoopInSeconds;
  this->positionY = this->positionY + this->speedY * deltaLoopInSeconds;

  this->previousLoopMillis = nowMillis;
}

void Ball::reset() {
  this->positionX = 1.0;
  this->positionY = 1.0;
  this->previousPositionX = -999.0;
  this->previousPositionY = -999.0;
  this->speedX = INITIAL_BALL_SPEED_X;
  this->speedY = INITIAL_BALL_SPEED_Y;
  this->previousLoopMillis = millis();
  this->hasTouchedWall = false;
  this->hasTouchedRacket = false;
  this->isOut = false;
}

void Game::reset() {
  this->player1->score = 0;
  this->player2->score = 0;
  this->player1->racket->positionX = 0;
  this->player1->racket->previousPositionX = -1;
  this->player1->racket->size = INITIAL_RACKET_SIZE;
  this->player2->racket->positionX = 0;
  this->player2->racket->previousPositionX = -1;
  this->player2->racket->size = INITIAL_RACKET_SIZE;
  this->winner = nullptr;
  this->ball.reset();
  this->state = GAME_INTRO;
  this->introStartMs = millis();
  // Bonus state
  this->lastHitter = nullptr;
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    this->fieldBonuses[i].active = false;
  }
  this->player1->inventoryCount = 0;
  this->player2->inventoryCount = 0;
  this->racket1Effect.active = false;
  this->racket2Effect.active = false;
  this->nextBonusSpawnTime = millis() + random(BONUS_SPAWN_MIN_MS, BONUS_SPAWN_MAX_MS + 1);
}