// Viewe LCM-UE050WV-AB40-L159A (ST72568, 800x480) on an ESP32-S3, driven
// natively over the internal RGB/DPI LCD peripheral -- SYNC-DE mode
// (DCLK, HSYNC, VSYNC, DE all driven by the ESP32-S3, see docs/wiring.md).
//
// Wired as RGB565 (16 data lines: the 5/6/5 MSBs of R/G/B). The 8 LSB
// lines (R0-R2, G0-G1, B0-B2) are tied to GND on the FPC adapter and are
// NOT connected to the ESP32-S3 -- see docs/wiring.md for why.

#include <Arduino_GFX_Library.h>

// ---- Adjust to the GPIOs actually wired on your ESP32-S3 board ----
#define TFT_DE     9
#define TFT_VSYNC  11
#define TFT_HSYNC  10
#define TFT_PCLK   8

// R3..R7 (5 MSBs of the 8-bit red bus, FPC pins 9-12 -> R3-R7)
#define TFT_R0 12
#define TFT_R1 13
#define TFT_R2 14
#define TFT_R3 15
#define TFT_R4 16
// G2..G7 (6 MSBs of the 8-bit green bus, FPC pins 15-20 -> G2-G7)
#define TFT_G0 17
#define TFT_G1 18
#define TFT_G2 21
#define TFT_G3 1
#define TFT_G4 2
#define TFT_G5 4
// B3..B7 (5 MSBs of the 8-bit blue bus, FPC pins 24-28 -> B3-B7)
#define TFT_B0 5
#define TFT_B1 6
#define TFT_B2 7
#define TFT_B3 35
#define TFT_B4 36

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4,
    1 /* hsync_polarity */, 4 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    1 /* vsync_polarity */, 4 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg, DCLK is negative polarity per datasheet */,
    14000000 /* prefer_speed -- start conservative, datasheet typ is 25MHz */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    800 /* width */, 480 /* height */, bus, 0 /* rotation */, true /* auto_flush */);

void setup() {
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(6);
  gfx->setCursor(220, 210);
  gfx->println("Hello World");
}

void loop() {
}
