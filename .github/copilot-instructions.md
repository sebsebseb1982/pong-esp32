# Copilot Instructions – pong-esp32

Two-player Pong on an ESP32 with a 64×64 HUB75 LED matrix, rotary encoder inputs, a buzzer, WiFi, and OTA updates. The project uses **PlatformIO** (Arduino framework).

## Build & Flash

```powershell
# Build only
platformio run

# Build & upload
platformio run --target upload

# Serial monitor (115200 baud)
platformio device monitor
```

All commands run from the workspace root where `platformio.ini` lives.

## Architecture

| Component | File(s) | Responsibility |
|-----------|---------|----------------|
| Game logic / physics | `game.h/.cpp` | Ball, Racket, Player classes; collision detection |
| Rendering | `user-interface.h/.cpp` | Pixel-level drawing to LED matrix |
| LED panel driver | `led-panel.h/.cpp` | HUB75 DMA initialization via ESP32-HUB75 lib |
| Colors | `colors.h/.cpp` | Static helpers wrapping `color565()` |
| Controls | `controls.h/.cpp` | Rotary encoders + direct buttons → paddle position |
| Audio | `sound.h/.cpp`, `buzzer.h/.cpp` | Ball event → tone playback via PWM |
| Network | `wifi-connection.h/.cpp`, `ota.h/.cpp` | WiFi station + ArduinoOTA daemon |
| Secrets | `secrets.h` | WiFi SSID & password (not committed) |

`src/main.cpp` owns the top-level `setup()` / `loop()` wiring; objects are instantiated there and each component's `loop()` is called every frame.

## Hardware

| Peripheral | Pin(s) |
|-----------|--------|
| HUB75 E pin | 32 |
| HUB75 driver | FM6126A @ 20 MHz |
| Buzzer (PWM ch 0) | 21 |
| P1 encoder A/B/btn | 33, 34, 35 |
| P1 shrink button | 2 |
| P2 encoder A/B/btn | 36, 39, 22 |
| P2 shrink button | 18 |

## Key Conventions

- **Old ESP32 Arduino ledc API** is in use (SDK < 3.0). Always use `ledcSetup(ch, freq, res)` / `ledcAttachPin(pin, ch)` / `ledcWrite(ch, duty)` / `ledcChangeFrequency(ch, freq, res)`. Do **not** use the new API (`ledcAttach`, `ledcWriteChannel`).
- **Delta-time physics**: ball position is `float`; movement uses `speed * deltaTime`.
- **Dirty-flag rendering**: `UserInterface` only redraws a ball/paddle when its position actually changes (compare previous vs. current rounded integer coordinates).
- **ISR functions** for rotary encoders must be declared `static IRAM_ATTR` and use only the static `*RotaryEncoderPtr` pointers; never access instance members from an ISR.
- **Playfield size**: `GAME_WIDTH = 58` (right 6 px reserved for score display), `GAME_HEIGHT = 64`.
- All classes use the same **setup() / loop()** contract; `loop()` must be callable every frame without blocking.

## Important Gotchas

- `secrets.h` contains plain-text WiFi credentials – **never commit real credentials**. Keep a local-only copy.
- `WiFiConnection::setup()` blocks for up to 10 s waiting for an IP; it performs a hard `ESP.restart()` on timeout.
- OTA has no password authentication; do not use on open networks.
- `loopScore()` in `Game` is currently empty – winner detection is not yet implemented.
- `LEDPanel::zob()` / `zob2()` are leftover debug methods; ignore them.
- `Buzzer::currentLevel` is declared but never used.
- `src/wifi.cpp` is empty – do not add logic there without understanding the intended split with `wifi-connection.cpp`.

## Dependencies (platformio.ini)

```ini
lib_deps =
    mrfaptastic/ESP32 HUB75 LED MATRIX PANEL DMA Display @ ^3.0.0
    igorantolic/Ai Esp32 Rotary Encoder @ ^1.7.0
    adafruit/Adafruit GFX Library @ ^1.11.0   ; transitive dep of HUB75 lib
```
