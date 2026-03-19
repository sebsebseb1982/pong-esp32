#include "sound.h"
#include "buzzer.h"

Sound::Sound(Game *game) {
  this->game = game;
}

void Sound::setup() {
}

void Sound::loop() {
  if (this->game->ball.hasTouchedWall) {
    Buzzer::playTone(1000, 8);
  } else if (this->game->ball.hasTouchedRacket) {
    Buzzer::playTone(2000, 8);
  }else if (this->game->ball.isOut) {
    Buzzer::playTone(500, 8);
  }
}