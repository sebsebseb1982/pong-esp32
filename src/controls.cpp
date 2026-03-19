#include "controls.h"

AiEsp32RotaryEncoder* Controls::player1RotaryEncoderPtr = nullptr;
AiEsp32RotaryEncoder* Controls::player2RotaryEncoderPtr = nullptr;

Controls::Controls(Game* game) {
  this->game = game;
  this->previousButton1 = false;
  this->previousButton2 = false;
  this->lastBoundarySize1 = INITIAL_RACKET_SIZE;
  this->lastBoundarySize2 = INITIAL_RACKET_SIZE;
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
    this->game->player1->racket,
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
    this->game->player2->racket,
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

void Controls::setupRotaryEncoder(AiEsp32RotaryEncoder* rotaryEncoder, Racket* racket, void (*ISR_callback)(void)) {
  rotaryEncoder->begin();
  rotaryEncoder->setup(ISR_callback);
  bool circleValues = false;
  rotaryEncoder->setBoundaries(
    0,
    GAME_WIDTH - racket->size - 1,
    circleValues);
  rotaryEncoder->setAcceleration(25);
}

void Controls::loop() {
  // During game over, any button press restarts the game
  if (this->game->state == GAME_OVER) {
    bool restartRequested =
      this->player1RotaryEncoder->isEncoderButtonClicked() ||
      this->player2RotaryEncoder->isEncoderButtonClicked() ||
      digitalRead(PLAYER_1_BUTTON_PIN) == HIGH ||
      digitalRead(PLAYER_2_BUTTON_PIN) == HIGH;
    if (restartRequested) {
      this->game->reset();
    }
    return;
  }

  // Player 1
  this->game->player1->racket->previousPositionX = this->game->player1->racket->positionX;
  if (this->player1RotaryEncoder->encoderChanged()) {
    this->game->player1->racket->positionX = this->player1RotaryEncoder->readEncoder();
  }
  if (this->player1RotaryEncoder->isEncoderButtonClicked()) {
    Serial.println("P1 rotary encoder clicked !");
  }

  // P1 action button: rising edge activates a bonus
  bool btn1Now = (digitalRead(PLAYER_1_BUTTON_PIN) == HIGH);
  if (btn1Now && !this->previousButton1) {
    this->game->activateBonus(this->game->player1);
  }
  this->previousButton1 = btn1Now;

  // Capture current size for UI dirty-flag, then sync encoder boundary if size changed
  this->game->player1->racket->previousSize = this->game->player1->racket->size;
  if (this->game->player1->racket->size != this->lastBoundarySize1) {
    this->player1RotaryEncoder->setBoundaries(
      0,
      GAME_WIDTH - this->game->player1->racket->size - 1,
      false);
    this->lastBoundarySize1 = this->game->player1->racket->size;
  }

  // Player 2
  this->game->player2->racket->previousPositionX = this->game->player2->racket->positionX;
  if (this->player2RotaryEncoder->encoderChanged()) {
    this->game->player2->racket->positionX = this->player2RotaryEncoder->readEncoder();
  }
  if (this->player2RotaryEncoder->isEncoderButtonClicked()) {
    Serial.println("P2 rotary encoder clicked !");
  }

  // P2 action button: rising edge activates a bonus
  bool btn2Now = (digitalRead(PLAYER_2_BUTTON_PIN) == HIGH);
  if (btn2Now && !this->previousButton2) {
    this->game->activateBonus(this->game->player2);
  }
  this->previousButton2 = btn2Now;

  // Capture current size for UI dirty-flag, then sync encoder boundary if size changed
  this->game->player2->racket->previousSize = this->game->player2->racket->size;
  if (this->game->player2->racket->size != this->lastBoundarySize2) {
    this->player2RotaryEncoder->setBoundaries(
      0,
      GAME_WIDTH - this->game->player2->racket->size - 1,
      false);
    this->lastBoundarySize2 = this->game->player2->racket->size;
  }
}
