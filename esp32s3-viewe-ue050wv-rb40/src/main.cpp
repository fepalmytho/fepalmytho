// Viewe UEDX80480050E-WB-B "Smart Display" shield (ESP32S3-MCN16R8 on
// board) driving a Viewe LCM-UEED050WV-RB40-L001A (ST7282A, 800x480,
// Transflective) plugged into its 40-pin RGB header. The shield's own
// bundled screen is NOT used here -- pin assignments below are the
// shield's fixed silkscreen GPIO map (see docs/wiring.md), not a free
// choice: this is a manufactured board, the pins can't be moved.
//
// Wired as RGB565 (16 data lines) -- this is a hardware limitation of
// the shield itself (only 16 of the 24 R/G/B lines are broken out to
// the ESP32-S3), not a choice made for this project.
//
// Backlight: the shield's LEDA/LEDK pins are only proven for VIEWE's own
// bundled screen. This project's screen is a different part with a
// different LED string (17V typ/70mA, see docs/wiring.md) -- its
// backlight is powered by an external TPS61169 boost driver, NOT by the
// shield. GPIO2 (LCD-BL-EN) drives that boost driver's enable/dimming
// pin instead of whatever backlight circuit the shield has on board.

#include <Arduino_GFX_Library.h>

// ---- Fixed by the Viewe shield's silkscreen/schematic, do not change ----
#define TFT_DE     40
#define TFT_VSYNC  41
#define TFT_HSYNC  39
#define TFT_PCLK   42

#define TFT_R0 45
#define TFT_R1 48
#define TFT_R2 47
#define TFT_R3 21
#define TFT_R4 14

#define TFT_G0 5
#define TFT_G1 6
#define TFT_G2 7
#define TFT_G3 15
#define TFT_G4 16
#define TFT_G5 4

#define TFT_B0 8
#define TFT_B1 3
#define TFT_B2 46
#define TFT_B3 9
#define TFT_B4 1

#define BACKLIGHT_EN 2   // LCD-BL-EN -> enable/dimming pin of the external TPS61169

// Timings from the panel's own datasheet section 5.4 (typ. values,
// 800x480 @ 60Hz). CLK/HSYNC/VSYNC are all "Negative polarity" per the
// panel's I/O table -- if the image rolls/shifts on first bring-up,
// flip the two polarity args below.
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
  pinMode(BACKLIGHT_EN, OUTPUT);
  digitalWrite(BACKLIGHT_EN, HIGH);

  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(6);
  gfx->setCursor(220, 210);
  gfx->println("Hello World");
}

void loop() {
}
