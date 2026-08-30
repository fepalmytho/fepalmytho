# Câblage E28GA-T-CW250-N <-> ESP32-S3

## Ce que c'est réellement

Le E28GA-T-CW250-N (Focus LCDs) est un **panneau nu** : dalle TFT 2,8" 240x320,
contrôleur **ST7789**, sortie sur nappe **FPC 40 broches, pas 0,5 mm, contact
côté inférieur ("bottom contact")**. Le connecteur recommandé par le
fabricant est un **Hirose FH12-40S-0,5SH(55)** (ZIF, contact inférieur).

Ce n'est **pas** un module breakout tout-en-un comme on en trouve chez
Adafruit/Waveshare : il n'y a ni level-shifter, ni driver de rétroéclairage,
ni straps de configuration sur la dalle elle-même. Il faut les ajouter.

## 1. Adaptateur nappe -> pas 2,54 mm

Chercher une carte adaptatrice **générique** :
« 40 pin 0.5mm pitch FPC to DIP breakout board, bottom contact ».
Le point important est **bottom contact** (contact inférieur), pour
correspondre à l'orientation de la nappe de cette dalle. Une carte "top
contact" ne fonctionnera pas dans le bon sens.

Exemples trouvés (vérifier la disponibilité au moment de l'achat) :
- FFC/FPC 40 Pin 0.5mm to DIP, Bottom Contact — Amazon (réf. B0BZ4YGRQP)
- FFC FPC 40 Pins 0.5mm Pitch to DIP 2.54mm PCB Converter Board (Bottom
  Contact) — Oz Robotics
- 40 Pin 0.5mm & 1mm pitch FPC to DIP Breakout — Tinkersphere

Ces cartes se contentent de sortir chacune des 40 broches sur du 2,54 mm ;
il faut ensuite câbler/souder soi-même les straps IM et le driver de
rétroéclairage (aucune intelligence sur la carte).

## 2. Mode d'interface : SPI "4-line 8-bit serial"

Le ST7789 de cette dalle expose plusieurs modes MCU sélectionnés par 4
broches de configuration matérielle IM0-IM3 (broches 3 à 6 de la nappe).
Pour du SPI classique (CS / DC / SCK / MOSI), il faut le mode
**"4-line 8-bit serial I/F"** :

| Broche | Signal | Valeur à souder |
|---|---|---|
| 3 | IM0 | GND |
| 4 | IM1 | VDDI (3,3V) |
| 5 | IM2 | VDDI (3,3V) |
| 6 | IM3 | GND |

Ces 4 broches se strappent une fois pour toutes sur l'adaptateur (petit fil
vers GND ou 3V3), ce ne sont pas des GPIO à piloter.

## 3. Table de câblage complète

| Nappe | Signal | Vers |
|---|---|---|
| 1, 2 | GND | GND |
| 3-6 | IM0-IM3 | straps, voir §2 |
| 7 | TE | NC (non utilisé ici) |
| 8 | VDDI | 3V3 |
| 9 | VDD | 3V3 |
| 10 | CSX | GPIO10 (CS) |
| 11 | DCX | GPIO9 (DC) |
| 12 | WRX | GPIO12 (SCK) |
| 13 | RDX | 3V3 (non utilisé, tirer au niveau haut) |
| 14-31 | DB0-DB17 | NC (mode parallèle non utilisé) |
| 32 | SDO | NC (pas de lecture du panneau) |
| 33 | SDA | GPIO11 (MOSI) |
| 34 | DOTCLK | GND (non utilisé, RGB I/F) |
| 35 | ENABLE | GND (non utilisé, RGB I/F) |
| 36 | HSYNC | GND (non utilisé, RGB I/F) |
| 37 | VSYNC | GND (non utilisé, RGB I/F) |
| 38 | RESX | GPIO8 (RST) |
| 39 | LEDK | Sortie - du driver de rétroéclairage |
| 40 | LEDA | Sortie + du driver de rétroéclairage |

Les GPIO ci-dessus sont ceux utilisés dans `src/main.cpp` ; adapter selon
les broches réellement câblées sur ta carte ESP32-S3.

## 4. Rétroéclairage : nécessite un driver boost

D'après la datasheet (§5.3), le rétroéclairage est composé de 8 LED
blanches montées en 2 chaînes de 4 LED en série, tension directe typique
**12 V (10,8 à 13,2 V)**, courant typique **40 mA**. C'est bien au-delà
de ce qu'une GPIO 3,3V de l'ESP32-S3 peut fournir directement : il faut un
**convertisseur boost à courant constant**, pas une simple résistance
série sur 3,3V.

Options :
- **Adafruit TPS61169 Constant Current Boost Converter for LEDs** — module
  tout fait, entrée basse tension, sortie boost jusqu'à plusieurs dizaines
  de volts, courant réglable, broche de gradation PWM pilotable directement
  par une GPIO 3,3V. C'est le plus simple à intégrer.
- Un module boost générique "constant current LED driver" (ex. autour des
  puces AL8861/ME2108/MP3308), à condition de pouvoir régler/limiter le
  courant à ~40 mA et d'accepter une entrée PWM ou EN pour le pilotage
  depuis l'ESP32-S3.

Brancher : `LEDA` (broche 40, anode) sur la sortie + du driver, `LEDK`
(broche 39, cathode) sur la sortie - du driver, et la broche PWM/EN du
driver sur `TFT_BL_PWM` (GPIO6 dans `src/main.cpp`).

## 5. Alimentation logique

`VDD` et `VDDI` acceptent tous les deux 3,3V (VDD: 2,4-3,3V typ 2,75V ;
VDDI: 1,65-3,3V typ 1,8V), donc l'alimentation 3V3 de l'ESP32-S3 convient
directement pour les deux, pas besoin de level-shifter sur les lignes SPI.
