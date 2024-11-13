#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  private:
    static unsigned long stopBeepMillis;
    static unsigned int currentLevel;
  public:
    static void setup();
    static void loop();
    static void on();
    static void off();
    static void setFrequency(int frequency);
    static void playTone(int frequency, int durationInMs);
};

#endif
