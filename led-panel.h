#ifndef LED_PANEL_H
#define LED_PANEL_H

#define SCREEN_WIDTH 64   // Number of pixels wide of each INDIVIDUAL panel module.
#define SCREEN_HEIGHT 64  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1     // Total number of panels chained one to another
#define standbyDelayInSeconds 10

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

class LEDPanel {
  private:
    unsigned long nextStandbyInMs;
  public:
    MatrixPanel_I2S_DMA* dma_display;
    LEDPanel(void);
    void setup();
    void loop();
    bool isStandby();
    void wakeUp();
    void zob(long zoby);
    void zob2(long zoby);
};

#endif