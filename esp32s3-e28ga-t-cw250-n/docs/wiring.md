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
- FFC/FPC 40 Pin 0.5mm to DIP, Bottom Contact — Amazon (réf. B0BZ4YGRQP,
  fabricant SB Components) — **validé** : malgré le "2,4 mm" affiché sur la
  fiche Amazon, la fiche technique officielle SB Components confirme un
  brochage de sortie en 2,54 mm (2x20 pastilles), compatible breadboard,
  et un connecteur SMT à levier bottom contact conforme à ce qu'il faut
  pour cette dalle. Seul bémol : très peu d'avis sur la fiche Amazon.
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

**Les 4 broches sont à souder, pas seulement IM0 et IM3.** 2 vont au GND
(IM0, IM3) et 2 vont au 3V3 (IM1, IM2) — c'est le fait que 2 d'entre elles
partagent la même destination (GND) qui peut donner l'impression qu'il y
en a moins à faire, mais les 4 fils sont nécessaires : si IM1 ou IM2 sont
laissées flottantes, le contrôleur ne saura pas dans quel mode démarrer et
l'écran ne s'initialisera pas (ou de façon aléatoire). Ce sont des straps
matériels fixes, soudés une fois sur l'adaptateur — ce ne sont pas des GPIO
à piloter depuis le code.

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

**Recommandation : Adafruit TPS61169 Constant Current Boost Converter for
LEDs** — https://www.adafruit.com/product/6354 (aussi chez Pimoroni/The Pi
Hut en UK/EU, souvent plus rapide/moins cher à livrer en France que
Adafruit direct US).

- Entrée 3-5V DC (donc alimentable directement en 5V ou en 3,3V).
- Boost jusqu'à ~40V / 400mA (pas les deux maxis en même temps).
- Courant réglable par mini-DIP switches : ~25mA par défaut (tout éteint),
  chaque switch ajoute +25/+50/+100/+200mA. Pour viser les ~40mA typiques
  du datasheet (répartis sur les 2 chaînes de LED en parallèle), démarrer
  avec le réglage par défaut (25mA, aucun switch activé) — déjà proche du
  besoin réel par chaîne — et n'activer le switch "+25mA" que si l'écran
  paraît trop sombre.
- Broche de gradation PWM pilotable directement par une GPIO 3,3V de
  l'ESP32-S3, pas de translation de niveau nécessaire.

Brancher : `LEDA` (broche 40, anode) sur la sortie + du driver, `LEDK`
(broche 39, cathode) sur la sortie - du driver, et la broche PWM/EN du
driver sur `TFT_BL_PWM` (GPIO6 dans `src/main.cpp`).

Alternative générique si indisponible : un module boost "constant current
LED driver" (puces type AL8861/ME2108/MP3308), à condition de pouvoir
régler/limiter le courant à ~40 mA et d'accepter une entrée PWM ou EN.

## 5. Alimentation

`VDD` et `VDDI` acceptent tous les deux 3,3V (VDD: 2,4-3,3V typ 2,75V ;
VDDI: 1,65-3,3V typ 1,8V), donc l'alimentation 3V3 de l'ESP32-S3 convient
directement pour les deux, pas besoin de level-shifter sur les lignes SPI.

Une seule alim 5V (chargeur USB-C) pour tout le projet fonctionne :
- Le 5V va sur l'entrée USB-C/5V-VIN de la carte ESP32-S3, qui régule elle
  même en interne le 3,3V pour le MCU — ce même rail 3,3V alimente `VDD`
  et `VDDI` de l'écran.
- Le driver de rétroéclairage (TPS61169) accepte 3-5V en entrée : on peut
  le piquer directement sur le même 5V (avant régulation) ou sur le 3,3V
  de la carte, les deux fonctionnent. Prendre le 5V limite légèrement les
  pertes du boost (moins de gain de tension à fournir jusqu'à ~12V).
- Attention au courant total disponible : un chargeur USB-C 5V/1A ou plus
  est largement suffisant (ESP32-S3 ~ quelques centaines de mA en pointe
  Wi-Fi, écran + logique quelques mA, backlight ~25-50mA côté boost, donc
  ~100-150mA côté 5V une fois le rendement du boost pris en compte).
