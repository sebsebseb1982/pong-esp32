#include "led-panel.h"
#include "colors.h"

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
  
  dma_display->drawRect(
    0,
    0,
    10,
    10,
    Colors::white(dma_display));
}

bool LEDPanel::isStandby() {
  return false;
}

void LEDPanel::zob(long zoby) {
  dma_display->clearScreen();
  //dma_display->fillScreen(Colors::black(dma_display));
  dma_display->drawRect(
    20,
    20,
    10,
    10,
    Colors::black(dma_display));
  dma_display->setCursor(20, 20);
  dma_display->setTextColor(Colors::white(dma_display));
  //dma_display->print(String(zoby, 1));
  dma_display->print(zoby);
}

void LEDPanel::wakeUp() {
  this->nextStandbyInMs = millis() + (standbyDelayInSeconds * 1000);
}