# Câblage Viewe LCM-UEED050WV-RB40-L001A <-> ESP32-S3

> **Le câblage physique se fait maintenant via le shield du vendeur**
> (`docs/shield.md`) plutôt qu'un adaptateur générique — ce fichier
> reste la référence pour les specs propres à l'écran (timings,
> rétroéclairage), mais les broches GPIO réelles sont celles du shield.

5,0" **Transflective** TFT, 800×480, contrôleur **ST7282A**, 220 cd/m²
typ (mode transmissif/backlight allumé), nappe FPC 40 broches / 0,5mm
pitch. Interface RGB parallèle 24-bit fixe, pas de mode à sélectionner
par straps (comme les écrans ST7789 SPI qu'on a câblés avant).

## Transflectif : ce que ça change, et ce que ça ne change pas

- Ça **ne change rien à la tension du rétroéclairage** : ce panneau
  utilise la même architecture de LED en série que les panneaux non
  transflectifs (voir plus bas), donc **le driver boost reste
  nécessaire**.
- Ça permet en revanche d'**éteindre ou baisser fortement le
  rétroéclairage en plein jour** (le mode réflectif prend le relais,
  contraste réflectif 7:1) — l'économie d'énergie vient de l'usage, pas
  du circuit.

## Pourquoi RGB565 (16 bits) au lieu de RGB888 (24 bits)

Le connecteur a 24 fils de données (R0-R7, G0-G7, B0-B7), mais on ne
câble que les bits de poids fort côté ESP32-S3 :

- Rouge : **R3-R7** (5 fils) -> R0-R2 reliés au **GND** sur l'adaptateur
- Vert : **G2-G7** (6 fils) -> G0-G1 au GND
- Bleu : **B3-B7** (5 fils) -> B0-B2 au GND

Total : **16 lignes data + PCLK + HSYNC + VSYNC + DE = 20 GPIO**.

## Table de câblage

| Nappe | Signal | Vers |
|---|---|---|
| 1 | LEDK | Cathode du driver de rétroéclairage |
| 2 | LEDA | Anode du driver de rétroéclairage |
| 3, 29, 36 | GND | GND |
| 4 | VCC (VDD interne) | 3V3 |
| 5-8 | R0-R3 | NC (reliés au GND sur l'adaptateur) |
| 9-12 | R4-R7 | GPIO ESP32-S3 (5 lignes, voir `src/main.cpp`) |
| 13-14 | G0-G1 | NC (reliés au GND) |
| 15-20 | G2-G7 | GPIO ESP32-S3 (6 lignes) |
| 21-23 | B0-B2 | NC (reliés au GND) |
| 24-28 | B3-B7 | GPIO ESP32-S3 (5 lignes) |
| 30 | CLK (PCLK) | GPIO ESP32-S3 |
| 31 | DISP | **3V3 fixe** (pas une GPIO — "Standby mode, normalement tiré haut") |
| 32 | HSYNC | GPIO ESP32-S3 |
| 33 | VSYNC | GPIO ESP32-S3 |
| 34 | DEN (DE) | GPIO ESP32-S3 |
| 35, 37-40 | NC | NC (Dummy, pas de tactile sur cette variante) |

Correspondance R0-R7/G0-G7/B0-B7 déduite de l'ordre standard du tableau
I/O (broches 5-28 consécutives) — à vérifier contre le schéma bloc avant
de souder si possible.

## Timings (datasheet section 5.4, valeurs typ., 800x480 @ 60Hz)

| Paramètre | Typ. | Unité |
|---|---|---|
| DCLK (pixel clock) | 32,4 | MHz (29-37,6 min-max) |
| HSYNC pulse width | 24 | DCLK |
| HSYNC back porch | 160 | DCLK |
| HSYNC front porch | 40 | DCLK |
| VSYNC pulse width | 2 | HSYNC |
| VSYNC back porch | 23 | HSYNC |
| VSYNC front porch | 37 | HSYNC |

CLK/HSYNC/VSYNC en polarité négative selon la datasheet. **Le code
démarre à 16MHz** par prudence (câblage prototypage sur fils Dupont) —
monter progressivement vers les 32,4MHz typiques une fois le câblage
validé et si les fils sont courts.

## Rétroéclairage : driver boost nécessaire

18 LED (6 en série × 3 branches parallèles) :

| | Valeur |
|---|---|
| Forward Voltage (VF) | 17V typ (16-18,6V) |
| Forward Current (IF) | 70mA typ |
| Puissance | 1,19W |

### Architecture d'alimentation retenue (12V bateau -> écran)

Un seul point de régulation partagé, pour que tout le reste du circuit
voie une tension stable malgré les variations du 12V bord (10,5-14,5V) :

```
12V bateau -> [buck 12V->5V, ex. Pololu D24V5Fx ou module auto MP1584] -> 5V stable
                                                                        -> ESP32-S3 (5V/VIN)
                                                                        -> Adafruit TPS61169 (IN)
                                                                             -> boost auto-ajusté (16-18,6V) -> LEDA/LEDK
```

- **Buck 12V->5V** : absorbe toute l'instabilité du bord. Un seul module
  à choisir avec une plage d'entrée large (couvrant au moins 9-16V).
- **Adafruit TPS61169** (boost constant-current, entrée 3-5V) alimenté
  par ce même 5V. **Ne pas chercher à fixer une tension de sortie** — le
  TPS61169 ajuste lui-même sa tension de sortie (dans la plage naturelle
  16-18,6V de ce panneau) pour maintenir le courant réglé par les DIP
  switches. C'est le fonctionnement normal d'un driver à courant
  constant, cohérent avec le fait que VF de la datasheet est une plage
  et non une valeur fixe.
- **Réglage DIP switch recommandé : 25mA (défaut) + 50mA = 75mA total**
  -> ~25mA par LED répartis sur les 3 branches parallèles (typ
  fabricant : 20mA/LED, max absolu : 120mA/LED) — marge confortable.
- Le courant réglé peut être ajusté (PWM sur la broche de gradation du
  TPS61169) pour tirer parti du mode transflectif : baisser/éteindre en
  plein jour, remonter au besoin la nuit.

## Alimentation logique

`VCC`/`VDD` accepte 3,0-3,6V (typ 3,3V) -> le 3,3V de l'ESP32-S3 convient
directement. Consommation logique typ 190mA sous VDD=3,3V (0,627W panel),
1,817W au total module avec backlight allumé.
