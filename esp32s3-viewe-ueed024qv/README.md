# Viewe UEED024QV-RH40-A003A + ESP32-S3

Projet PlatformIO pour piloter l'écran TFT Viewe UEED024QV-RH40-A003A
(2,41", 240x320, ST7789) depuis un ESP32-S3 en SPI, avec un simple test
"Hello World".

- `docs/wiring.md` : straps IM, câblage complet, rétroéclairage.
- `src/main.cpp` : initialisation SPI + affichage "Hello World".

## Build

```
pio run -e esp32-s3-devkitc-1
pio run -t upload
```

Adapter les définitions `TFT_*` en haut de `src/main.cpp` aux GPIO
réellement câblés sur l'adaptateur reçu avec l'écran.
