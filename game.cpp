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
  this->ball.loop();
  this->loopCollisions();
  this->loopScore();
}

Racket::Racket(int positionY) {
  this->positionX = 0;
  this->positionY = positionY;
  this->lastPositionX = -1;
  this->lastPositionY = -1;
  this->size = INITIAL_RACKET_SIZE;
}

Player::Player(Racket *racket) {
  this->score = 0;
  this->racket = racket;
}

Ball::Ball(void) {
  this->positionX = 1.0;
  this->positionY = 1.0;
  this->lastPositionX = this->positionX;
  this->lastPositionY = this->positionY;
  this->speedX = 16.3;
  this->speedY = 22.2;
  this->lastLoopMillis = millis();
  this->hasTouchedWall = false;
  this->hasTouchedRacket = false;
  this->isOut = false;
}

void Ball::reboundXIfNeeded(float limitX, TypeLimit typeLimit) {
  if (this->speedX < 0.0 && this->positionX <= limitX && typeLimit == MIN) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = limitX + (limitX - this->positionX);
  } else if (this->speedX > 0.0 && this->positionX >= limitX && typeLimit == MAX) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = limitX - (this->positionX - limitX);
  }
}

bool Ball::reboundYIfNeeded(Racket *racket, TypeLimit typeLimit) {
  bool missedBall = false;
  float limitY = typeLimit == MIN ? (racket->positionY + 1) * 1.0 : (racket->positionY - 1) * 1.0;
  float racketMinX = racket->positionX * 1.0;
  float racketMaxX = (racket->positionX + racket->size) * 1.0;
  if (this->speedY < 0.0 && this->positionY <= limitY && typeLimit == MIN) {
    if (racketMinX <= this->positionX && this->positionX <= racketMaxX) {
      this->hasTouchedRacket = true;
    } else {
      this->isOut = true;
      missedBall = true;
    }
    this->speedY = this->speedY * -1.0;
    this->positionY = limitY + (limitY - this->positionY);
  } else if (this->speedY > 0.0 && this->positionY >= limitY && typeLimit == MAX) {
    if (racketMinX <= this->positionX && this->positionX <= racketMaxX) {
      this->hasTouchedRacket = true;
    } else {
      this->isOut = true;
      missedBall = true;
    }
    this->speedY = this->speedY * -1.0;
    this->positionY = limitY - (this->positionY - limitY);
  }
  return missedBall;
}

void Ball::loop() {
  unsigned long nowMillis = millis();
  this->hasTouchedWall = false;
  this->hasTouchedRacket = false;
  this->isOut = false;
  this->lastPositionX = this->positionX;
  this->lastPositionY = this->positionY;

  float deltaLoopInSeconds = (nowMillis * 1.0 - this->lastLoopMillis * 1.0) / 1000.0;
  this->positionX = this->positionX + this->speedX * deltaLoopInSeconds;
  this->positionY = this->positionY + this->speedY * deltaLoopInSeconds;

  this->lastLoopMillis = nowMillis;
}