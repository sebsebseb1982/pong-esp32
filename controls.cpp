#include "controls.h"

AiEsp32RotaryEncoder* Controls::player1RotaryEncoderPtr = nullptr;
AiEsp32RotaryEncoder* Controls::player2RotaryEncoderPtr = nullptr;

Controls::Controls(Game* game) {
  this->game = game;
}

void Controls::setup() {

  // Player 1
  pinMode(PLAYER_1_BUTTON_PIN, INPUT_PULLDOWN);

  this->player1RotaryEncoder = new AiEsp32RotaryEncoder(
    PLAYER_1_ROTARY_ENCODER_A_PIN,
    PLAYER_1_ROTARY_ENCODER_B_PIN,
    PLAYER_1_ROTARY_ENCODER_BUTTON_PIN,
    ROTARY_ENCODER_VCC_PIN,
    ROTARY_ENCODER_STEPS);
  Controls::player1RotaryEncoderPtr = this->player1RotaryEncoder;
  setupRotaryEncoder(
    this->player1RotaryEncoder,
    Controls::readPlayer1RotaryEncoderISR);

  // Player 2
  pinMode(PLAYER_2_BUTTON_PIN, INPUT_PULLDOWN);

  this->player2RotaryEncoder = new AiEsp32RotaryEncoder(
    PLAYER_2_ROTARY_ENCODER_A_PIN,
    PLAYER_2_ROTARY_ENCODER_B_PIN,
    PLAYER_2_ROTARY_ENCODER_BUTTON_PIN,
    ROTARY_ENCODER_VCC_PIN,
    ROTARY_ENCODER_STEPS);
  Controls::player2RotaryEncoderPtr = this->player2RotaryEncoder;
  setupRotaryEncoder(
    this->player2RotaryEncoder,
    Controls::readPlayer2RotaryEncoderISR);
}

void IRAM_ATTR Controls::readPlayer1RotaryEncoderISR() {
  if (Controls::player1RotaryEncoderPtr != nullptr) {
    Controls::player1RotaryEncoderPtr->readEncoder_ISR();
  }
}

void IRAM_ATTR Controls::readPlayer2RotaryEncoderISR() {
  if (Controls::player2RotaryEncoderPtr != nullptr) {
    Controls::player2RotaryEncoderPtr->readEncoder_ISR();
  }
}

void Controls::setupRotaryEncoder(AiEsp32RotaryEncoder* rotaryEncoder, void (*ISR_callback)(void)) {
  rotaryEncoder->begin();
  rotaryEncoder->setup(ISR_callback);
  bool circleValues = false;
  rotaryEncoder->setBoundaries(
    0,
    GAME_WIDTH - INITIAL_RACKET_SIZE - 1,
    circleValues);
  rotaryEncoder->setAcceleration(25);
}

void Controls::loop() {
  // Player 1
  this->game->player1->racket->previousPositionX = this->game->player1->racket->positionX;
  if (this->player1RotaryEncoder->encoderChanged()) {
    this->game->player1->racket->positionX = this->player1RotaryEncoder->readEncoder();
  }
  if (this->player1RotaryEncoder->isEncoderButtonClicked()) {
    Serial.println("P1 rotary encoder clicked !");
  }

  if (digitalRead(PLAYER_1_BUTTON_PIN) == HIGH) {
    this->game->player1->racket->size = INITIAL_RACKET_SIZE / 2;
  } else {
    this->game->player1->racket->size = INITIAL_RACKET_SIZE;
  }

  // Player 2
  this->game->player2->racket->previousPositionX = this->game->player2->racket->positionX;
  if (this->player2RotaryEncoder->encoderChanged()) {
    this->game->player2->racket->positionX = this->player2RotaryEncoder->readEncoder();
  }
  if (this->player2RotaryEncoder->isEncoderButtonClicked()) {
    Serial.println("P2 rotary encoder clicked !");
  }

  if (digitalRead(PLAYER_2_BUTTON_PIN) == HIGH) {
    this->game->player2->racket->size = INITIAL_RACKET_SIZE / 2;
  } else {
    this->game->player2->racket->size = INITIAL_RACKET_SIZE;
  }
}
