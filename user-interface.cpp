#include "user-interface.h"
#include "colors.h"
#include <math.h>

UserInterface::UserInterface(Game *game, LEDPanel *ledPanel) {
  this->game = game;
  this->ledPanel = ledPanel;
}
void UserInterface::setup() {
}
void UserInterface::loop() {
  drawBall();
}
void UserInterface::drawBall() {
  uint16_t ballColor = Colors::white(this->ledPanel->dma_display);
  uint16_t backgroundColor = Colors::black(this->ledPanel->dma_display);

  int lastPixelX = round(this->game->ball.lastPositionX);
  int lastPixelY = round(this->game->ball.lastPositionY);
  int currentPixelX = round(this->game->ball.positionX);
  int currentPixelY = round(this->game->ball.positionY);

  if (lastPixelX != currentPixelX || lastPixelY != currentPixelY) {
    this->ledPanel->dma_display->drawPixel(
      lastPixelX,
      lastPixelY,
      backgroundColor);

    this->ledPanel->dma_display->drawPixel(
      currentPixelX,
      currentPixelY,
      ballColor);
  }
}