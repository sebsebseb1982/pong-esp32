#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game.h"
#include "led-panel.h"

class UserInterface {
  private:
    Game *game;
    LEDPanel *ledPanel;
    void drawBall();
  public:
    UserInterface(Game *game, LEDPanel *ledPanel);
    void setup();
    void loop();
};

#endif