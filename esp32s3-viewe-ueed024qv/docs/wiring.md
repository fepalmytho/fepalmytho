# Câblage Viewe UEED024QV-RH40-A003A <-> ESP32-S3

2,41" 240x320, contrôleur ST7789, nappe FPC 40 broches / 0,5mm pitch,
tactile capacitif présent sur la nappe (broches 1-4) mais **non utilisé**
dans ce projet (broches CTP laissées non connectées).

## Mode d'interface : SPI "4-wire 8-bit serial"

Straps matériels à souder sur l'adaptateur (broches 38-40 de la nappe) :

| Broche | Signal | Valeur à souder |
|---|---|---|
| 38 | IM0 | GND |
| 39 | IM1 | VCC (3,3V) |
| 40 | IM2 | VCC (3,3V) |

## Table de câblage

| Nappe | Signal | Vers |
|---|---|---|
| 5, 16, 29, 37 | GND | GND |
| 6 | IOVCC | 3V3 |
| 7 | VCC | 3V3 |
| 9 | SPI_CS | GPIO10 (CS) |
| 10 | SPI_SCL | GPIO12 (SCK) |
| 11 | SPI_RS | GPIO9 (DC) |
| 12 | MCU_RD | NC (non utilisé en SPI) |
| 13 | SPI_SDA | GPIO11 (MOSI) |
| 14 | SPI_SDO | NC (pas de lecture du panneau) |
| 15 | RESET | GPIO8 (RST) |
| 17-32 | DB0-DB15 | NC (mode parallèle non utilisé) |
| 33 | LED-A | Alim rétroéclairage (voir plus bas) |
| 34-36 | LED-K | Vers le drain du transistor de rétroéclairage |
| 1-4 | CTP (tactile) | NC (non utilisé) |
| 8 | TE | NC (non utilisé) |

## Rétroéclairage : pas de boost, juste résistance + transistor

4 LED en parallèle, Vf typ 3,0V (2,9-3,2V), ~20mA chacune (80mA total) —
contrairement au Focus LCDs, **pas besoin de driver boost 12V**. Câbler :

- `LED-A` (broche 33, anode commune) -> 5V à travers une résistance série
  d'environ 25Ω (calcul : (5V-3V)/0,08A ≈ 25Ω, ~0,16W, résistance standard)
- `LED-K` (broches 34-36, reliées ensemble) -> drain d'un MOSFET canal N
  logic-level, source à la masse, grille sur `TFT_BL` (GPIO6 dans
  `src/main.cpp`) — permet l'on/off et la gradation PWM

Le code fourni fait juste un `digitalWrite(TFT_BL, HIGH)` (allumage simple,
pas de gradation) pour le test "Hello World" — passer en `ledcWrite(...)`
si besoin de gradation plus tard.

## Alimentation logique

`VCC` (2,8-3,3V) et `IOVCC` (1,8-3,3V) acceptent tous les deux le 3,3V de
l'ESP32-S3 directement, pas de level-shifter nécessaire sur les lignes SPI.
