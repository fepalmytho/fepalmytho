// E28GA-T-CW250-N (ST7789, 240x320) driven from an ESP32-S3 over SPI,
// in "4-line 8-bit serial" MCU mode (IM3:IM2:IM1:IM0 strapped to 0:1:1:0
// on the FPC adapter board -- see docs/wiring.md).
//
// SDA (FPC pin 33) is MOSI-only in this project: we never read from the
// panel, so SDO (pin 32) and the parallel DBx pins are left unconnected.

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ---- Adjust to the GPIOs actually wired on your ESP32-S3 board ----
#define TFT_SCK   12   // -> FPC pin 12 (WRX)
#define TFT_MOSI  11   // -> FPC pin 33 (SDA)
#define TFT_CS    10   // -> FPC pin 10 (CSX)
#define TFT_DC     9   // -> FPC pin 11 (DCX)
#define TFT_RST    8   // -> FPC pin 38 (RESX)
#define TFT_BL_PWM 6   // -> PWM/dimming input of the backlight boost driver

SPIClass spi(FSPI);
Adafruit_ST7789 tft(&spi, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL_PWM, OUTPUT);
  ledcAttach(TFT_BL_PWM, 20000, 8);   // 20 kHz PWM, 8-bit duty
  ledcWrite(TFT_BL_PWM, 200);         // ~78% brightness; drives the boost driver's dimming pin

  spi.begin(TFT_SCK, -1 /* MISO unused */, TFT_MOSI, TFT_CS);
  tft.init(240, 320);
  tft.setSPISpeed(40000000);
  tft.setRotation(0);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("E28GA-T-CW250-N");
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("ESP32-S3 / ST7789 / SPI");

  tft.fillRect(10, 60, 60, 60, ST77XX_RED);
  tft.fillRect(80, 60, 60, 60, ST77XX_GREEN);
  tft.fillRect(150, 60, 60, 60, ST77XX_BLUE);
}

void loop() {
}
