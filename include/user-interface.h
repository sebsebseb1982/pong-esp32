#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game.h"
#include "led-panel.h"

class UserInterface {
  private:
    Game *game;
    LEDPanel *ledPanel;
    GameState previousState;
    int prevBonusPixelX[MAX_FIELD_BONUSES];
    int prevBonusPixelY[MAX_FIELD_BONUSES];
    bool prevBonusActive[MAX_FIELD_BONUSES];
    void drawBall();
    void drawRacket(Racket *racket);
    void drawFieldBonuses();
    void drawBonusInventory();
    void drawScores();
    void drawAScore(Player *player, int positionY);
    void drawInterface();
    void drawIntro();
    void drawGameOver();
  public:
    UserInterface(Game *game, LEDPanel *ledPanel);
    void setup();
    void loop();
};

#endif