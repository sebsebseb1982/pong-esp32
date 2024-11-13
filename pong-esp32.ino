#include "wifi-connection.h"
#include "ota.h"
#include "led-panel.h"
#include "colors.h"
#include "AiEsp32RotaryEncoder.h"
#include "buzzer.h"
#include "game.h"
#include "user-interface.h"

#define ROTARY_ENCODER_A_PIN_P1 34
#define ROTARY_ENCODER_B_PIN_P1 33
#define ROTARY_ENCODER_BUTTON_PIN_P1 35


#define ROTARY_ENCODER_A_PIN_P2 36
#define ROTARY_ENCODER_B_PIN_P2 39
#define ROTARY_ENCODER_BUTTON_PIN_P2 22

/*
#define ROTARY_ENCODER_A_PIN_P2 15
#define ROTARY_ENCODER_B_PIN_P2 2
#define ROTARY_ENCODER_BUTTON_PIN_P2 0
*/

#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

LEDPanel ledPanel(8);
Game game;
UserInterface ui(&game, &ledPanel);

AiEsp32RotaryEncoder rotaryEncoderP1 = AiEsp32RotaryEncoder(
  ROTARY_ENCODER_A_PIN_P1,
  ROTARY_ENCODER_B_PIN_P1,
  ROTARY_ENCODER_BUTTON_PIN_P1,
  ROTARY_ENCODER_VCC_PIN,
  ROTARY_ENCODER_STEPS);


AiEsp32RotaryEncoder rotaryEncoderP2 = AiEsp32RotaryEncoder(
  ROTARY_ENCODER_A_PIN_P2,
  ROTARY_ENCODER_B_PIN_P2,
  ROTARY_ENCODER_BUTTON_PIN_P2,
  ROTARY_ENCODER_VCC_PIN,
  ROTARY_ENCODER_STEPS);

void rotary_onButtonClick() {
  static unsigned long lastTimePressed = 0;
  //ignore multiple press in that time milliseconds
  if (millis() - lastTimePressed < 500) {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
  
}

void rotary_loop_p1() {
  //dont print anything unless value changed
  if (rotaryEncoderP1.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoderP1.readEncoder());
    //ledPanel.dma_display->setBrightness8(rotaryEncoder.readEncoder());

    ledPanel.zob(rotaryEncoderP1.readEncoder());
  }
  if (rotaryEncoderP1.isEncoderButtonClicked()) {
    rotary_onButtonClick();
  } 
}

void rotary_loop_p2() {
  //dont print anything unless value changed
  if (rotaryEncoderP2.encoderChanged()) {
    Serial.print("Value: ");
    Serial.println(rotaryEncoderP2.readEncoder());
    //ledPanel.dma_display->setBrightness8(rotaryEncoder.readEncoder());

    ledPanel.zob2(rotaryEncoderP2.readEncoder());
    Buzzer::setFrequency(rotaryEncoderP2.readEncoder()*100);
  }
  if (rotaryEncoderP2.isEncoderButtonClicked()) {
    //rotary_onButtonClick();
    Buzzer::on();
  } else {
    Buzzer::off();
  }
}

void IRAM_ATTR readEncoderISR_p1() {
  rotaryEncoderP1.readEncoder_ISR();
}
void IRAM_ATTR readEncoderISR_p2() {
  rotaryEncoderP2.readEncoder_ISR();
}



void setup() {
  Serial.begin(115200);
  Serial.println("setup()");
  pinMode(2, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);
  WiFiConnection::setup();
  Buzzer::setup();
  OTA::setup();
  ledPanel.setup();
  game.setup();
  game.newGame();
  ui.setup();
  
  //we must initialize rotary encoder
  rotaryEncoderP1.begin();
  rotaryEncoderP1.setup(readEncoderISR_p1);
  //set boundaries and if values should cycle or not
  //in this example we will set possible values between 0 and 1000;
  bool circleValues = false;
  rotaryEncoderP1.setBoundaries(0, 57, circleValues);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoderP1.setAcceleration(25);  //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration



  //we must initialize rotary encoder
  rotaryEncoderP2.begin();
  rotaryEncoderP2.setup(readEncoderISR_p2);
  //set boundaries and if values should cycle or not
  //in this example we will set possible values between 0 and 1000;
  rotaryEncoderP2.setBoundaries(0, 57, circleValues);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoderP2.setAcceleration(25);  //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}

void loop() {
  //Serial.println("loop()");
  Buzzer::loop();

  WiFiConnection::loop();
  //OTA::loop();
  ledPanel.loop();
  rotary_loop_p1();
  rotary_loop_p2();
  game.loop();
  ui.loop();

  int buttonState2 = digitalRead(2);
  //Serial.println(buttonState2);

  if (buttonState2 == LOW) {
    //Serial.println("setup()");
  } else {
    Serial.println("HIGH 2");
  }

  int buttonState18 = digitalRead(18);
  //Serial.println(buttonState18);

  if (buttonState18 == LOW) {
    //Serial.println("setup()");
  } else {
    Serial.println("HIGH 18");
  }
}