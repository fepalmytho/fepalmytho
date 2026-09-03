// Viewe LCM-UEED050WV-RB40-L001A (ST7282A, 800x480, Transflective) on an
// ESP32-S3, driven natively over the internal RGB/DPI LCD peripheral --
// no MCU/serial mode to select, the bus is fixed RGB (see docs/wiring.md).
//
// Wired as RGB565 (16 data lines: the 5/6/5 MSBs of R/G/B). The 8 LSB
// lines (R0-R2, G0-G1, B0-B2) are tied to GND on the FPC adapter and are
// NOT connected to the ESP32-S3.

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

// Timings from datasheet section 5.4 (typ. values, 800x480 @ 60Hz).
// CLK/HSYNC/VSYNC are all "Negative polarity" per the I/O table -- if the
// image rolls/shifts on first bring-up, flip the two polarity args below.
Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4,
    0 /* hsync_polarity */, 40 /* hsync_front_porch */, 24 /* hsync_pulse_width */, 160 /* hsync_back_porch */,
    0 /* vsync_polarity */, 37 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 23 /* vsync_back_porch */,
    1 /* pclk_active_neg, CLK is negative polarity per datasheet */,
    16000000 /* prefer_speed -- start conservative, datasheet typ is 32.4MHz */);

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
