#include "wifi-connection.h"
#include "ota.h"
#include "controls.h"
#include "led-panel.h"
#include "colors.h"
#include "buzzer.h"
#include "game.h"
#include "user-interface.h"
#include "sound.h"

LEDPanel ledPanel;
Game game;
UserInterface ui(&game, &ledPanel);
Sound sound(&game);
Controls controls(&game);

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  WiFiConnection::setup();
  Buzzer::setup();
  OTA::setup();
  ledPanel.setup();
  game.setup();
  ui.setup();
  sound.setup();
  controls.setup();
}

void loop() {
  WiFiConnection::loop();
  OTA::loop();
  controls.loop();
  ledPanel.loop();
  game.loop();
  ui.loop();
  sound.loop();
  Buzzer::loop();
}