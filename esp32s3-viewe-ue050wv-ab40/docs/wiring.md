# Câblage Viewe LCM-UE050WV-AB40-L159A <-> ESP32-S3

5,0" IPS TFT, 800×480, contrôleur **ST72568**, 1000 nits, nappe FPC 40
broches / 0,5mm pitch. **Interface RGB parallèle 24-bit, pas de SPI, pas
de mode à sélectionner par straps** (contrairement aux écrans ST7789
qu'on a câblés avant) : le bus est fixe, il faut juste driver
correctement DCLK/HSYNC/VSYNC/DE + les données.

## Pourquoi RGB565 (16 bits) au lieu de RGB888 (24 bits)

Le connecteur a bien 24 fils de données (R0-R7, G0-G7, B0-B7), mais on ne
câble que les bits de poids fort côté ESP32-S3 :

- Rouge : **R3-R7** (5 fils) -> les 3 bits de poids faible R0-R2 sont
  reliés au **GND** sur l'adaptateur, pas connectés à l'ESP32
- Vert : **G2-G7** (6 fils) -> G0-G1 au GND
- Bleu : **B3-B7** (5 fils) -> B0-B2 au GND

Total : **16 lignes data + PCLK + HSYNC + VSYNC + DE = 20 GPIO**, au lieu
de 28 en RGB888 complet. Le contrôleur latch quand même tout le bus à
chaque pixel ; les bits non pilotés valent toujours 0, ce qui réduit juste
la précision couleur (65536 couleurs au lieu de 16,7M) — largement
suffisant pour un affichage de chiffres + code couleur.

## Table de câblage

| Nappe | Signal | Vers |
|---|---|---|
| 1 | VLED- | Cathode du driver de rétroéclairage (voir plus bas) |
| 2 | VLED+ | Anode du driver de rétroéclairage |
| 3, 29, 36 | GND | GND |
| 4 | VDD | 3V3 |
| 5-8 | R0-R3 | NC (reliés au GND sur l'adaptateur) |
| 9-12 | R4-R7 | GPIO ESP32-S3 (5 lignes, voir `src/main.cpp`) |
| 13-14 | G0-G1 | NC (reliés au GND) |
| 15-20 | G2-G7 | GPIO ESP32-S3 (6 lignes) |
| 21-23 | B0-B2 | NC (reliés au GND) |
| 24-28 | B3-B7 | GPIO ESP32-S3 (5 lignes) |
| 30 | PCLK | GPIO ESP32-S3 |
| 31 | DISP | **3V3 fixe** (pas une GPIO — "Standby mode, normalement tiré haut") |
| 32 | HSYNC | GPIO ESP32-S3 |
| 33 | VSYNC | GPIO ESP32-S3 |
| 34 | DE | GPIO ESP32-S3 |
| 35 | NC/RESET | NC (marqué "Dummy" dans la datasheet) |
| 37-40 | NC | NC (pas de tactile sur cette variante) |

Attention : cette correspondance R0-R7/G0-G7/B0-B7 = broches 5-28 est
déduite de l'ordre standard du tableau I/O (R0-R7 puis G0-G7 puis B0-B7
consécutifs) — à vérifier contre le schéma bloc de la datasheet avant de
souder.

## Timings (depuis la datasheet, section 5.4.2)

| Paramètre | Typ. | Unité |
|---|---|---|
| DCLK (pixel clock) | 25 | MHz (23-27 min-max) |
| HSYNC back porch | 8 | DCLK |
| HSYNC front porch | 8 | DCLK |
| HSYNC pulse width | 4 | DCLK |
| VSYNC back porch | 8 | HSYNC |
| VSYNC front porch | 8 | HSYNC |
| VSYNC pulse width | 4 | HSYNC |

DCLK à polarité négative (donnée latchée sur front descendant) — géré par
le paramètre `pclk_active_neg` dans `src/main.cpp`.

**Le code démarre à 14MHz par prudence**, pas les 25MHz typiques de la
datasheet : sur du câblage prototypage (fils Dupont, adaptateur FPC), un
bus parallèle à 25MHz est sensible aux problèmes d'intégrité de signal
(bruit, pixels qui scintillent). Une fois le câblage validé et si les fils
sont courts, on peut monter progressivement vers 25MHz.

## Rétroéclairage : driver boost obligatoire, plus costaud que les écrans précédents

18 LED (6 en série × 3 branches parallèles), tension directe **typique
18V** (jusqu'à ~20,4V en pire cas), courant total typique **120mA**,
soit **~2,16W**. Il faut un vrai driver LED boost dédié capable de
18-20V / 150mA (le petit TPS61169 regardé pour d'autres écrans n'est pas
dimensionné pour cette puissance).

Avec le 12V du bord disponible sur le bateau, ce boost n'a plus qu'à
monter de 12V à 18-20V (ratio ×1,5-1,7, largement plus facile qu'un boost
depuis du 5V) — un module boost ajustable à courant constant (type
XL6009) dimensionné pour ~3W avec marge convient.

## Alimentation logique

`VDD` accepte 2,8-3,6V (typ 3,3V) -> le 3,3V de l'ESP32-S3 convient
directement. Consommation logique ~30mA (0,099W), l'essentiel de la
consommation du module vient du rétroéclairage (2,16W sur 2,259W total).
