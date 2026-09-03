# Viewe LCM-UEED050WV-RB40-L001A + ESP32-S3

Projet PlatformIO pour piloter l'écran TFT transflectif Viewe
LCM-UEED050WV-RB40-L001A (5,0", 800x480, RGB parallèle, ST7282A, 220
cd/m²) depuis un ESP32-S3, via le périphérique RGB/DPI natif de
l'ESP32-S3 (pas de puce de conversion SPI->parallèle nécessaire).

- `docs/wiring.md` : câblage RGB565 (20 GPIO), timings, driver de
  rétroéclairage boost 17-18,6V nécessaire (le transflectif ne dispense
  pas du boost, il permet juste d'éteindre le backlight en plein jour).
- `src/main.cpp` : initialisation du bus RGB via Arduino_GFX + affichage
  "Hello World".

## Build

```
pio run -e esp32-s3-devkitc-1
pio run -t upload
```

Adapter les définitions `TFT_*` en haut de `src/main.cpp` aux GPIO
réellement câblés, et vérifier que ta carte ESP32-S3 expose bien 20 GPIO
libres (certaines variantes réservent des broches à la PSRAM/flash).
