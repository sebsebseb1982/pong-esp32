#include <Arduino.h>
#include "game.h"
#include "buzzer.h"

Game::Game(void) {
}

void Game::setup() {
}

void Game::loop() {
  this->ball.loop();
}

void Game::newGame() {
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
}

void Ball::loop() {
  unsigned long nowMillis = millis();
  this->hasTouchedWall = false;
  this->lastPositionX = this->positionX;
  this->lastPositionY = this->positionY;

  float deltaLoopInSeconds = (nowMillis * 1.0 - this->lastLoopMillis * 1.0) / 1000.0;
  this->positionX = this->positionX + this->speedX * deltaLoopInSeconds;
  this->positionY = this->positionY + this->speedY * deltaLoopInSeconds;

  float maxX = (GAME_WIDTH - 1) * 1.0;
  float maxY = (GAME_HEIGHT - 1) * 1.0;

  if (this->positionX <= 0.0) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = this->positionX * -1.0;
  } else if (this->positionX >= maxX) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = maxX - (this->positionX - maxX);
  }

  if (this->positionY <= 0.0) {
    this->hasTouchedWall = true;
    this->speedY = this->speedY * -1.0;
    this->positionY = this->positionY * -1.0;
  } else if (this->positionY >= maxY) {
    this->hasTouchedWall = true;
    this->speedY = this->speedY * -1.0;
    this->positionY = maxY - (this->positionY - maxY);
  }

  this->lastLoopMillis = nowMillis;
}