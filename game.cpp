#include <Arduino.h>
#include "game.h"
#include "buzzer.h"

Game::Game(void) {
  this->player1 = new Player(new Racket(0));
  this->player2 = new Player(new Racket(GAME_HEIGHT - 1));
}

void Game::setup() {
}

void Game::loopCollisions() {
  // Walls
  this->ball.reboundXIfNeeded(0.0, MIN);
  this->ball.reboundXIfNeeded((GAME_WIDTH - 1) * 1.0, MAX);

  // Racket
  this->player2->score += this->ball.reboundYIfNeeded(this->player1->racket, MIN) ? 1 : 0;
  this->player1->score += this->ball.reboundYIfNeeded(this->player2->racket, MAX) ? 1 : 0;
}

void Game::loopScore() {
}

void Game::loop() {
  this->player1->racket->loop();
  this->player2->racket->loop();
  this->ball.loop();
  this->loopCollisions();
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
}

Ball::Ball(void) {
  this->positionX = 1.0;
  this->positionY = 1.0;
  this->previousPositionX = this->positionX;
  this->previousPositionY = this->positionY;
  this->speedX = 16.3;
  this->speedY = 22.2;
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
    this->speedX = this->speedX * -0.9;
    if (this->speedX > 0) {
      this->speedX = max(this->speedX, (float)(MINIMUM_SPEED * 1.0));
    } else {
      this->speedX = min(this->speedX, (float)(MINIMUM_SPEED * -1.0));
    }
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
    this->speedY = this->speedY * -0.9;
    if (this->speedY > 0) {
      this->speedY = max(this->speedY, (float)(MINIMUM_SPEED * 1.0));
    } else {
      this->speedY = min(this->speedY, (float)(MINIMUM_SPEED * -1.0));
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