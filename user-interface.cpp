#include "user-interface.h"
#include "colors.h"
#include <math.h>

UserInterface::UserInterface(Game *game, LEDPanel *ledPanel) {
  this->game = game;
  this->ledPanel = ledPanel;
}
void UserInterface::setup() {
  drawInterface();
  loop();
}
void UserInterface::loop() {
  drawBall();
  drawRacket(this->game->player1->racket);
  drawRacket(this->game->player2->racket);
  drawScores();
}

void UserInterface::drawInterface() {
  uint16_t interfaceColor = Colors::lightGrey(this->ledPanel->dma_display);
  this->ledPanel->dma_display->drawLine(
    GAME_WIDTH,
    0,
    GAME_WIDTH,
    SCREEN_HEIGHT,
    interfaceColor);
}

void UserInterface::drawAScore(Player *player, int positionY) {
  uint16_t scoredPointsColor = Colors::white(this->ledPanel->dma_display);
  uint16_t unscoredPointsColor = Colors::darkGrey(this->ledPanel->dma_display);

  this->ledPanel->dma_display->fillRect(
    SCREEN_WIDTH - 4,
    positionY,
    3,
    MAXIMUM_POINTS / 3,
    unscoredPointsColor);

  for (int i = 0; i < 3; i++) {
    if (player->score > i * (MAXIMUM_POINTS / 3)) {
      this->ledPanel->dma_display->drawLine(
        SCREEN_WIDTH - 4 + i,
        positionY,
        SCREEN_WIDTH - 4 + i,
        positionY + min(player->score - (i * (MAXIMUM_POINTS / 3)), 7) - 1,
        scoredPointsColor);
    }
  }
}

void UserInterface::drawScores() {
  drawAScore(this->game->player1, 1);
  drawAScore(this->game->player2, SCREEN_HEIGHT - (MAXIMUM_POINTS / 3) - 1);
}

void UserInterface::drawRacket(Racket *racket) {
  uint16_t racketColor = Colors::white(this->ledPanel->dma_display);
  uint16_t backgroundColor = Colors::black(this->ledPanel->dma_display);

  bool hasChangedPosition = racket->previousPositionX != racket->positionX;

  if (hasChangedPosition) {
    this->ledPanel->dma_display->drawLine(
      racket->previousPositionX,
      racket->positionY,
      racket->previousPositionX + racket->size,
      racket->positionY,
      backgroundColor);
    this->ledPanel->dma_display->drawLine(
      racket->positionX,
      racket->positionY,
      racket->positionX + racket->size,
      racket->positionY,
      racketColor);
  }
}

void UserInterface::drawBall() {
  uint16_t ballColor = Colors::white(this->ledPanel->dma_display);
  uint16_t backgroundColor = Colors::black(this->ledPanel->dma_display);

  int previousPixelX = round(this->game->ball.previousPositionX);
  int previousPixelY = round(this->game->ball.previousPositionY);
  int currentPixelX = round(this->game->ball.positionX);
  int currentPixelY = round(this->game->ball.positionY);

  if (previousPixelX != currentPixelX || previousPixelY != currentPixelY) {
    this->ledPanel->dma_display->drawPixel(
      previousPixelX,
      previousPixelY,
      backgroundColor);

    this->ledPanel->dma_display->drawPixel(
      currentPixelX,
      currentPixelY,
      ballColor);
  }
}