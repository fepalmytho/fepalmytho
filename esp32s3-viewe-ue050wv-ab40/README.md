# Viewe LCM-UE050WV-AB40-L159A + ESP32-S3

Projet PlatformIO pour piloter l'écran TFT Viewe LCM-UE050WV-AB40-L159A
(5,0", 800x480, RGB parallèle, ST72568, 1000 nits) depuis un ESP32-S3, en
utilisant le périphérique RGB/DPI natif de l'ESP32-S3 (pas de puce de
conversion SPI->parallèle nécessaire).

- `docs/wiring.md` : câblage RGB565 (20 GPIO), timings, driver de
  rétroéclairage boost 18-20V nécessaire.
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
