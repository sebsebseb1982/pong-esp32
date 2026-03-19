#ifndef SOUND_H
#define SOUND_H
#include "game.h"

class Sound {
  private:
    Game *game;
  public:
    Sound(Game *game);
    void setup();
    void loop();
};

#endif