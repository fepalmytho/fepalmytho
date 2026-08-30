# E28GA-T-CW250-N + ESP32-S3

Projet PlatformIO pour piloter l'écran TFT Focus LCDs E28GA-T-CW250-N
(2,8", 240x320, ST7789) depuis un ESP32-S3 en SPI.

- `docs/wiring.md` : adaptateur nappe, straps de configuration du
  contrôleur, driver de rétroéclairage, table de câblage complète.
- `src/main.cpp` : initialisation SPI + PWM rétroéclairage + test pattern.

## Build

```
pio run -e esp32-s3-devkitc-1
pio run -t upload
```

Adapter les définitions `TFT_*` en haut de `src/main.cpp` aux GPIO
réellement câblés.
