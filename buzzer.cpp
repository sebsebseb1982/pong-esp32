#include "esp32-hal-ledc.h"
#include "buzzer.h"

#define pwmChannel 0          //Choisit le canal 0
#define defaultFrequency 500  //Fréquence PWM de 1 KHz
#define resolution 8          // Résolution de 8 bits, 256 valeurs possibles

#define buzzerPin 21

unsigned long Buzzer::stopBeepMillis = 0;
unsigned int Buzzer::currentLevel = 0;

void Buzzer::setup() {
  ledcAttach(
    buzzerPin,
    defaultFrequency,
    resolution);
}

void Buzzer::on() {
  Serial.println("on()");
  ledcWriteChannel(
    pwmChannel,
    127);
  Buzzer::stopBeepMillis = 0;
}

void Buzzer::off() {
  ledcWriteChannel(
    pwmChannel,
    0);
}

void Buzzer::loop() {
  if (Buzzer::stopBeepMillis != 0 && millis() > stopBeepMillis) {
    off();
  }
}

void Buzzer::setFrequency(int frequency) {
  Serial.print("setFrequency(");
  Serial.print(frequency);
  Serial.print(")");
  ledcChangeFrequency(
    buzzerPin,
    frequency,
    resolution);
}


void Buzzer::playTone(int frequency, int durationInMs) {
  ledcAttach(
    buzzerPin,
    frequency,
    resolution);
  Buzzer::stopBeepMillis = millis() + durationInMs;
  on();
}
