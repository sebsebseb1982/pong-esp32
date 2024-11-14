#include "esp32-hal-ledc.h"
#include "buzzer.h"

#define pwmChannel 0          //Choisit le canal 0
#define defaultFrequency 500  //Fréquence PWM de 1 KHz
#define resolution 8          // Résolution de 8 bits, 256 valeurs possibles

#define buzzerPin 21

unsigned long Buzzer::stopBeepMillis = 0UL;
unsigned int Buzzer::currentLevel = 0;

void Buzzer::setup() {
  ledcAttach(
    buzzerPin,
    defaultFrequency,
    resolution);
}

void Buzzer::on() {
  ledcWriteChannel(
    pwmChannel,
    127);
}

void Buzzer::off() {
  ledcWriteChannel(
    pwmChannel,
    0);
  Buzzer::stopBeepMillis = 0UL;
}

void Buzzer::loop() {
  if (Buzzer::stopBeepMillis != 0UL && millis() > stopBeepMillis) {
    off();
  }
}

void Buzzer::setFrequency(int frequency) {
  ledcChangeFrequency(
    buzzerPin,
    frequency,
    resolution);
}

void Buzzer::playTone(int frequency, int durationInMs) {
  setFrequency(frequency);
  Buzzer::stopBeepMillis = millis() + durationInMs;
  on();
}