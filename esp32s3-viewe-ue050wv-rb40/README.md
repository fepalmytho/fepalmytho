# Viewe LCM-UEED050WV-RB40-L001A + shield ESP32-S3 UEDX80480050E-WB-B

Projet PlatformIO pour piloter l'écran TFT transflectif Viewe
LCM-UEED050WV-RB40-L001A (5,0", 800x480, RGB parallèle, ST7282A, 220
cd/m²), acheté séparément, branché sur le shield ESP32-S3
UEDX80480050E-WB-B fourni par le vendeur (le shield a son propre écran
tactile par défaut, non utilisé ici).

- `docs/wiring.md` : specs de l'écran (RGB565, timings, driver de
  rétroéclairage boost 17-18,6V nécessaire — le transflectif ne dispense
  pas du boost, il permet juste d'éteindre le backlight en plein jour).
- `docs/shield.md` : le shield lui-même — GPIO fixes (non modifiables,
  carte manufacturée), alimentation (5V uniquement, pas de 12V natif),
  pourquoi son circuit backlight n'est pas utilisé pour cet écran, et
  combien de GPIO restent libres pour le reste du projet.
- `src/main.cpp` : initialisation du bus RGB via Arduino_GFX avec les
  vraies broches du shield + affichage "Hello World".

## Build

```
pio run -e esp32-s3-devkitc-1
pio run -t upload
```

Les broches `TFT_*` dans `src/main.cpp` sont fixées par le shield
(voir `docs/shield.md`), pas à modifier sauf erreur constatée au
premier test.
