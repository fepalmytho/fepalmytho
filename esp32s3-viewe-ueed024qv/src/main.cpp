// Viewe UEED024QV-RH40-A003A (ST7789, 240x320) on an ESP32-S3, SPI
// "4-wire 8-bit serial" MCU mode (IM2:IM1:IM0 strapped to 1:1:0 on the
// FPC adapter -- see docs/wiring.md). Touch (CTP) pins exist on the FPC
// but are left unconnected, this project doesn't use touch.

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ---- Adjust to the GPIOs actually wired on your ESP32-S3 board ----
#define TFT_SCK   12   // -> FPC pin 10 (SPI_SCL)
#define TFT_MOSI  11   // -> FPC pin 13 (SPI_SDA)
#define TFT_CS    10   // -> FPC pin 9  (SPI_CS)
#define TFT_DC     9   // -> FPC pin 11 (SPI_RS)
#define TFT_RST    8   // -> FPC pin 15 (RESET)
#define TFT_BL     6   // -> gate of the backlight switching transistor

SPIClass spi(FSPI);
Adafruit_ST7789 tft(&spi, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);   // backlight on (needs the transistor + resistor from docs/wiring.md)

  spi.begin(TFT_SCK, -1 /* MISO unused */, TFT_MOSI, TFT_CS);
  tft.init(240, 320);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 140);
  tft.println("Hello");
  tft.setCursor(20, 175);
  tft.println("World");
}

void loop() {
}
