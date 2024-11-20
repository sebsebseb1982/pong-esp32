#ifndef CONTROLS_H
#define CONTROLS_H

#include "game.h"
#include "AiEsp32RotaryEncoder.h"

#define PLAYER_1_ROTARY_ENCODER_A_PIN 33
#define PLAYER_1_ROTARY_ENCODER_B_PIN 34
#define PLAYER_1_ROTARY_ENCODER_BUTTON_PIN 35
#define PLAYER_1_BUTTON_PIN 2

#define PLAYER_2_ROTARY_ENCODER_A_PIN 36
#define PLAYER_2_ROTARY_ENCODER_B_PIN 39
#define PLAYER_2_ROTARY_ENCODER_BUTTON_PIN 22
#define PLAYER_2_BUTTON_PIN 18

#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

class Controls {
private:
  Game *game;
  AiEsp32RotaryEncoder *player1RotaryEncoder;
  AiEsp32RotaryEncoder *player2RotaryEncoder;
  static AiEsp32RotaryEncoder *player1RotaryEncoderPtr;
  static AiEsp32RotaryEncoder *player2RotaryEncoderPtr;
  void setupRotaryEncoder(AiEsp32RotaryEncoder *rotaryEncoder, void (*ISR_callback)(void));
  static void readPlayer1RotaryEncoderISR();
  static void readPlayer2RotaryEncoderISR();
public:
  Controls(Game *game);
  void setup();
  void loop();
};

#endif