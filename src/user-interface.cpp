#include "user-interface.h"
#include "colors.h"
#include <math.h>

UserInterface::UserInterface(Game *game, LEDPanel *ledPanel) {
  this->game = game;
  this->ledPanel = ledPanel;
  this->previousState = GAME_INTRO;
  this->prevRacket1Color = 0;
  this->prevRacket2Color = 0;
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    this->prevBonusPixelX[i] = -1;
    this->prevBonusPixelY[i] = -1;
    this->prevBonusActive[i] = false;
  }
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    this->prevObstacles[i].active = false;
  }
}

void UserInterface::setup() {
  drawIntro();
}

void UserInterface::loop() {
  GameState currentState = this->game->state;

  // React to state transitions
  if (currentState != this->previousState) {
    switch (currentState) {
      case GAME_INTRO:
        drawIntro();
        break;
      case GAME_STARTING:
        // Intro over: clear screen and prepare play area (ball stationary, no P1/P2)
        this->ledPanel->dma_display->fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT,
          Colors::black(this->ledPanel->dma_display));
        this->ledPanel->dma_display->drawLine(59, 0, 59, SCREEN_HEIGHT - 1,
          Colors::black(this->ledPanel->dma_display));
        drawInterface();
        this->game->player1->racket->previousPositionX = -1;
        this->game->player2->racket->previousPositionX = -1;
        for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
          this->prevBonusPixelX[i] = -1;
          this->prevBonusPixelY[i] = -1;
          this->prevBonusActive[i] = false;
        }
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          this->prevObstacles[i].active = false;
        }
        break;
      case GAME_PLAYING:
        // GAME_STARTING already set up the play area; nothing to (re)do here
        break;
      case GAME_OVER:
        // Clear inventory column before drawing game-over screen
        this->ledPanel->dma_display->drawLine(59, 0, 59, SCREEN_HEIGHT - 1,
          Colors::black(this->ledPanel->dma_display));
        drawGameOver();
        break;
    }
    this->previousState = currentState;
  }

  // Per-frame rendering
  switch (currentState) {
    case GAME_INTRO:
      drawRacket(this->game->player1->racket);
      drawRacket(this->game->player2->racket);
      break;
    case GAME_STARTING:
    case GAME_PLAYING:
      drawFieldBonuses();
      drawObstacles();
      drawBall();
      drawRacket(this->game->player1->racket);
      drawRacket(this->game->player2->racket);
      drawBonusInventory();
      drawScores();
      break;
    case GAME_OVER:
      break;
  }
}

void UserInterface::drawIntro() {
  this->ledPanel->dma_display->fillScreen(Colors::black(this->ledPanel->dma_display));
  drawInterface();

  // "P1" in green, just below the top racket (y=0)
  this->ledPanel->dma_display->setTextSize(1);
  this->ledPanel->dma_display->setTextColor(Colors::green(this->ledPanel->dma_display));
  this->ledPanel->dma_display->setCursor(23, 3);
  this->ledPanel->dma_display->print("P1");

  // "P2" in red, just above the bottom racket (y=63)
  this->ledPanel->dma_display->setTextColor(Colors::red(this->ledPanel->dma_display));
  this->ledPanel->dma_display->setCursor(23, 54);
  this->ledPanel->dma_display->print("P2");

  // Force rackets to be drawn by the per-frame loop
  this->game->player1->racket->previousPositionX = -1;
  this->game->player2->racket->previousPositionX = -1;
}

void UserInterface::drawGameOver() {
  this->ledPanel->dma_display->fillRect(0, 0, GAME_WIDTH, GAME_HEIGHT,
    Colors::black(this->ledPanel->dma_display));

  bool player1Won = (this->game->winner == this->game->player1);
  const char *winnerName = player1Won ? "P1" : "P2";
  uint16_t winnerColor = player1Won
    ? Colors::green(this->ledPanel->dma_display)
    : Colors::red(this->ledPanel->dma_display);

  // "P1" or "P2" at text size 2 — 2 chars × 12 px = 24 px wide; center in 58 px → x=17
  this->ledPanel->dma_display->setTextSize(2);
  this->ledPanel->dma_display->setTextColor(winnerColor);
  this->ledPanel->dma_display->setCursor(17, 14);
  this->ledPanel->dma_display->print(winnerName);

  // "WIN!" at text size 2 — 4 chars × 12 px = 48 px wide; center in 58 px → x=5
  this->ledPanel->dma_display->setTextColor(Colors::white(this->ledPanel->dma_display));
  this->ledPanel->dma_display->setCursor(5, 32);
  this->ledPanel->dma_display->print("WIN!");

  // Hint at size 1 — "Press btn" = 9 chars × 6 px = 54 px; center → x=2
  this->ledPanel->dma_display->setTextSize(1);
  this->ledPanel->dma_display->setTextColor(Colors::grey(this->ledPanel->dma_display));
  this->ledPanel->dma_display->setCursor(2, 54);
  this->ledPanel->dma_display->print("Press btn");
}

void UserInterface::drawInterface() {
  uint16_t interfaceColor = Colors::lightGrey(this->ledPanel->dma_display);
  this->ledPanel->dma_display->drawLine(
    GAME_WIDTH,
    0,
    GAME_WIDTH,
    SCREEN_HEIGHT,
    interfaceColor);
}

void UserInterface::drawAScore(Player *player, int positionY) {
  uint16_t scoredPointsColor = Colors::white(this->ledPanel->dma_display);
  uint16_t unscoredPointsColor = Colors::darkGrey(this->ledPanel->dma_display);

  this->ledPanel->dma_display->fillRect(
    SCREEN_WIDTH - 4,
    positionY,
    3,
    MAXIMUM_POINTS / 3,
    unscoredPointsColor);

  for (int i = 0; i < 3; i++) {
    if (player->score > i * (MAXIMUM_POINTS / 3)) {
      this->ledPanel->dma_display->drawLine(
        SCREEN_WIDTH - 4 + i,
        positionY,
        SCREEN_WIDTH - 4 + i,
        positionY + min(player->score - (i * (MAXIMUM_POINTS / 3)), 7) - 1,
        scoredPointsColor);
    }
  }
}

void UserInterface::drawScores() {
  drawAScore(this->game->player1, 1);
  drawAScore(this->game->player2, SCREEN_HEIGHT - (MAXIMUM_POINTS / 3) - 1);
}

void UserInterface::drawRacket(Racket *racket) {
  // Determine color based on active bonus effect on this racket
  uint16_t racketColor;
  if (racket == this->game->player1->racket && this->game->racket1Effect.active) {
    racketColor = (this->game->racket1Effect.type == BONUS_SHRINK_ENEMY)
      ? Colors::red(this->ledPanel->dma_display)
      : Colors::green(this->ledPanel->dma_display);
  } else if (racket == this->game->player2->racket && this->game->racket2Effect.active) {
    racketColor = (this->game->racket2Effect.type == BONUS_SHRINK_ENEMY)
      ? Colors::red(this->ledPanel->dma_display)
      : Colors::green(this->ledPanel->dma_display);
  } else {
    racketColor = Colors::white(this->ledPanel->dma_display);
  }
  uint16_t backgroundColor = Colors::black(this->ledPanel->dma_display);

  uint16_t &prevColor = (racket == this->game->player1->racket) ? this->prevRacket1Color : this->prevRacket2Color;
  bool needRedraw = racket->previousPositionX != racket->positionX
                 || racket->previousSize != racket->size
                 || racketColor != prevColor;

  if (needRedraw) {
    prevColor = racketColor;
    this->ledPanel->dma_display->drawLine(
      racket->previousPositionX,
      racket->positionY,
      racket->previousPositionX + racket->previousSize,
      racket->positionY,
      backgroundColor);
    this->ledPanel->dma_display->drawLine(
      racket->positionX,
      racket->positionY,
      racket->positionX + racket->size,
      racket->positionY,
      racketColor);
  }
}

void UserInterface::drawFieldBonuses() {
  for (int i = 0; i < MAX_FIELD_BONUSES; i++) {
    BonusItem &b = this->game->fieldBonuses[i];
    int curX = b.active ? (int)b.positionX : -1;
    int curY = b.active ? (int)b.positionY : -1;

    bool changed = (b.active != this->prevBonusActive[i]) ||
                   (curX    != this->prevBonusPixelX[i])  ||
                   (curY    != this->prevBonusPixelY[i]);
    if (!changed) continue;

    // Erase previous pixel
    if (this->prevBonusActive[i]) {
      this->ledPanel->dma_display->drawPixel(
        this->prevBonusPixelX[i],
        this->prevBonusPixelY[i],
        Colors::black(this->ledPanel->dma_display));
    }
    // Draw new pixel
    if (b.active) {
      uint16_t bonusColor;
      if (b.type == BONUS_SHRINK_ENEMY) {
        bonusColor = Colors::red(this->ledPanel->dma_display);
      } else if (b.type == BONUS_ENLARGE_SELF) {
        bonusColor = Colors::green(this->ledPanel->dma_display);
      } else {
        bonusColor = Colors::white(this->ledPanel->dma_display);
      }
      this->ledPanel->dma_display->drawPixel(curX, curY, bonusColor);
    }
    this->prevBonusActive[i] = b.active;
    this->prevBonusPixelX[i] = curX;
    this->prevBonusPixelY[i] = curY;
  }
}

void UserInterface::drawBonusInventory() {
  // P1 inventory bar at x=59, y=9..15 (between scores, fills downward)
  for (int i = 0; i < MAX_INVENTORY; i++) {
    uint16_t color = Colors::black(this->ledPanel->dma_display);
    if (i < this->game->player1->inventoryCount) {
      color = (this->game->player1->inventory[i] == BONUS_SHRINK_ENEMY)
        ? Colors::red(this->ledPanel->dma_display)
        : (this->game->player1->inventory[i] == BONUS_ENLARGE_SELF)
          ? Colors::green(this->ledPanel->dma_display)
          : Colors::white(this->ledPanel->dma_display);
    }
    this->ledPanel->dma_display->drawPixel(59, 9 + i, color);
  }
  // P2 inventory bar at x=59, y=49..55 (between scores, fills upward)
  for (int i = 0; i < MAX_INVENTORY; i++) {
    uint16_t color = Colors::black(this->ledPanel->dma_display);
    if (i < this->game->player2->inventoryCount) {
      color = (this->game->player2->inventory[i] == BONUS_SHRINK_ENEMY)
        ? Colors::red(this->ledPanel->dma_display)
        : (this->game->player2->inventory[i] == BONUS_ENLARGE_SELF)
          ? Colors::green(this->ledPanel->dma_display)
          : Colors::white(this->ledPanel->dma_display);
    }
    this->ledPanel->dma_display->drawPixel(59, 55 - i, color);
  }
}

void UserInterface::drawObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    ObstacleItem &obs = this->game->obstacles[i];
    PrevObstacle &prev = this->prevObstacles[i];
    bool changed = (obs.active != prev.active) ||
                   (obs.active && (obs.x != prev.x || obs.y != prev.y ||
                                   obs.w != prev.w || obs.h != prev.h));
    if (!changed) continue;

    if (prev.active) {
      this->ledPanel->dma_display->fillRect(
        prev.x, prev.y, prev.w, prev.h,
        Colors::black(this->ledPanel->dma_display));
    }
    if (obs.active) {
      this->ledPanel->dma_display->fillRect(
        obs.x, obs.y, obs.w, obs.h,
        Colors::white(this->ledPanel->dma_display));
    }
    prev.active = obs.active;
    prev.x = obs.x;
    prev.y = obs.y;
    prev.w = obs.w;
    prev.h = obs.h;
  }
}

void UserInterface::drawBall() {
  uint16_t ballColor = Colors::white(this->ledPanel->dma_display);
  uint16_t backgroundColor = Colors::black(this->ledPanel->dma_display);

  int previousPixelX = round(this->game->ball.previousPositionX);
  int previousPixelY = round(this->game->ball.previousPositionY);
  int currentPixelX = round(this->game->ball.positionX);
  int currentPixelY = round(this->game->ball.positionY);

  if (previousPixelX != currentPixelX || previousPixelY != currentPixelY) {
    this->ledPanel->dma_display->drawPixel(
      previousPixelX,
      previousPixelY,
      backgroundColor);

    this->ledPanel->dma_display->drawPixel(
      currentPixelX,
      currentPixelY,
      ballColor);
  }
}