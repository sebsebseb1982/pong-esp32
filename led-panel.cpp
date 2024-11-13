#include "led-panel.h"
#include "colors.h"

#define RACKET_SIZE 6

LEDPanel::LEDPanel(int toto) {
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    SCREEN_WIDTH,   // module width
    SCREEN_HEIGHT,  // module height
    PANEL_CHAIN     // Chain length
  );

  mxconfig.gpio.e = 32;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  //mxconfig.latch_blanking = 4;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;

  // Display Setup
  this->dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  this->dma_display->begin();
}

void LEDPanel::setup() {
  dma_display->setBrightness8(100);  //0-255
  //dma_display->setBrightness8(255);  //0-255
  dma_display->clearScreen();
  dma_display->fillScreen(Colors::black(dma_display));
}

void LEDPanel::loop() {
    
}

bool LEDPanel::isStandby() {
  return false;
}

void LEDPanel::zob(long zoby) {
  dma_display->drawLine(
    0,
    0,
    SCREEN_WIDTH,
    0,
    Colors::black(dma_display));

  dma_display->drawLine(
    zoby,
    0,
    zoby + RACKET_SIZE,
    0,
    Colors::white(dma_display));
}
void LEDPanel::zob2(long zoby) {
dma_display->drawLine(
    0,
    SCREEN_HEIGHT - 1,
    SCREEN_WIDTH,
    SCREEN_HEIGHT - 1,
    Colors::black(dma_display));

  dma_display->drawLine(
    zoby,
    SCREEN_HEIGHT - 1,
    zoby + RACKET_SIZE,
    SCREEN_HEIGHT - 1,
    Colors::white(dma_display));
}

void LEDPanel::wakeUp() {
  this->nextStandbyInMs = millis() + (standbyDelayInSeconds * 1000);
}