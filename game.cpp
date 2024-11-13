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

  /*
  if (this->lastPositionX <= 0.0 || this->lastPositionX >= GAME_WIDTH - 1) {
    this->speedX = this->speedX * -1.0;
    Buzzer::playTone(300, 5);
  }

  if (this->lastPositionY <= 0.0 || this->lastPositionY >= GAME_HEIGHT - 1) {
    this->speedY = this->speedY * -1.0;
    Buzzer::playTone(300, 5);
  }
  */

  float deltaLoopInSeconds = (nowMillis * 1.0 - this->lastLoopMillis * 1.0) / 1000.0;
  this->positionX = this->positionX + this->speedX * deltaLoopInSeconds;
  this->positionY = this->positionY + this->speedY * deltaLoopInSeconds;

  if (this->positionX <= 0.0) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = this->positionX * -1.0;
  } else if (this->positionX >= GAME_WIDTH - 1) {
    this->hasTouchedWall = true;
    this->speedX = this->speedX * -1.0;
    this->positionX = ((GAME_WIDTH * 1.0) - 1.0) - (this->positionX - (GAME_WIDTH * 1.0));
  }


  if (this->positionY <= 0.0) {
    this->hasTouchedWall = true;
    this->speedY = this->speedY * -1.0;
    this->positionY = this->positionY * -1.0;
  } else if (this->positionY >= GAME_HEIGHT - 1) {
    this->hasTouchedWall = true;
    this->speedY = this->speedY * -1.0;
    this->positionY = ((GAME_HEIGHT * 1.0) - 1.0) - (this->positionY - (GAME_HEIGHT * 1.0));
  }

  this->lastLoopMillis = nowMillis;
}