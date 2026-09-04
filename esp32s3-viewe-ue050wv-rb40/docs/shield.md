# Le shield ESP32-S3 Viewe UEDX80480050E-WB-B

Analyse de la fiche technique du shield fourni par le vendeur
(`PLCM-UEDX80480050E-WB-B`), à distinguer de l'écran lui-même (celui
documenté dans `wiring.md` reste le tien, pas celui vendu par défaut
avec ce shield).

## Ce que c'est

Une vraie carte assemblée avec un **ESP32S3-MCN16R8** dessus (dual-core,
240MHz, Wi-Fi/BLE, **16MB flash, 8MB PSRAM octal**), un connecteur FPC
40 broches RGB compatible avec notre écran (même format 40-pin RGB
24bit), un lecteur microSD, un port USB-C (programmation + alim), et un
connecteur tactile I2C séparé (GT911) pour l'écran tactile fourni par
défaut — **pas utilisé ici puisque ton écran n'a pas de tactile**.

Comme c'est une carte manufacturée, **les broches ne sont plus un choix**
— elles sont figées par le circuit imprimé (contrairement aux projets
précédents où on choisissait les GPIO nous-mêmes sur un adaptateur
générique). `src/main.cpp` a été mis à jour avec les vraies broches.

## Alimentation du shield : toujours 5V, pas de 12V natif

La fiche est claire (section 2.4) :
- **Power Voltage : 4,0 à 5,5V DC, typ 5V** — via le port USB-C
- Alimentation recommandée : **5V 1A DC**

**Le shield n'accepte pas le 12V directement.** L'architecture qu'on
avait prévue reste donc nécessaire : buck 12V (bord) → 5V, branché sur
l'entrée USB-C (ou directement sur les pins 5V/GND si le shield les
expose sur le header, à vérifier visuellement) du shield.

## Rétroéclairage : NE PAS COMPTER SUR LE SHIELD POUR TON ÉCRAN

Le shield a des broches `LEDA`/`LEDK` sur son connecteur écran, et une
GPIO2 (`LCD-BL-EN`) pour activer le rétroéclairage. **Mais ces broches
sont calibrées pour l'écran que Viewe vend avec ce shield par défaut**
(400 cd/m², specs propres à cet écran-là), pas garanties compatibles
avec notre écran séparé (LCM-UEED050WV-RB40-L001A, VF 17V typ/70mA — une
architecture LED différente). Rien dans la fiche ne confirme un vrai
boost réglable à bord capable de fournir 17-18,6V pour un écran tiers —
GPIO2 ressemble plutôt à un simple enable on/off qu'à un contrôleur de
boost générique.

**Décision retenue : ignorer le circuit backlight du shield pour ton
écran.** Utiliser le montage externe déjà prévu (TPS61169 alimenté par
le même 5V, en courant constant ~75mA — voir `wiring.md`), et piloter
son entrée de gradation avec **GPIO2** au lieu du câblage LEDA/LEDK du
shield. Le connecteur écran du shield sert uniquement pour le bus RGB
(données + synchro), pas pour l'alimentation du rétroéclairage.

Si tu veux en avoir le cœur net, le lien "Schematic Diagram (PDF)" cité
dans la fiche (section 5, Related Documents) confirmerait s'il y a un
vrai boost à bord ou juste un enable — je n'ai pas pu y accéder
directement (site fabricant bloqué depuis cet environnement), tu peux me
l'envoyer si tu veux vérifier plus précisément.

## Table des GPIO fixes du shield

| GPIO | Fonction fixe |
|---|---|
| CHIP-EN | Bouton Reset |
| GPIO0 | Bouton Boot / LED RGB (WS2812B) |
| GPIO1 | LCD Blue B4 |
| GPIO2 | LCD Backlight Enable |
| GPIO3 | LCD Blue B1 |
| GPIO4 | LCD Green G5 |
| GPIO5 | LCD Green G0 |
| GPIO6 | LCD Green G1 |
| GPIO7 | LCD Green G2 |
| GPIO8 | LCD Blue B0 |
| GPIO9 | LCD Blue B3 |
| GPIO10-13 | SD Card (CS/MOSI/SCLK/MISO) |
| GPIO14 | LCD Red R4 |
| GPIO15 | LCD Green G3 |
| GPIO16 | LCD Green G4 |
| **GPIO17** | **Libre** |
| GPIO18 | Touch Interrupt (GT911) |
| GPIO19 | Touch I2C SDA |
| GPIO20 | Touch I2C SCL |
| GPIO21 | LCD Red R3 |
| GPIO38 | Touch Reset |
| GPIO39 | LCD HSYNC |
| GPIO40 | LCD DE |
| GPIO41 | LCD VSYNC |
| GPIO42 | LCD PCLK |
| GPIO43 | UART TX (debug, via CH340C) |
| GPIO44 | UART RX (debug, via CH340C) |
| GPIO45 | LCD Red R0 |
| GPIO46 | LCD Blue B2 |
| GPIO47 | LCD Red R2 |
| GPIO48 | LCD Red R1 |

GPIO22-37 n'apparaissent pas dans la table : sur ce module ESP32-S3 en
variante PSRAM octale (8MB), ces broches sont réservées en interne au
bus mémoire et ne sont pas disponibles, point sur lequel on n'a pas la
main.

## Combien de GPIO dispo pour le reste du projet ?

- **1 broche explicitement libre : GPIO17.**
- **+4 broches récupérables puisque tu n'utilises pas le tactile** (ton
  écran n'en a pas) : GPIO18, 19, 20, 38 — rien d'autre sur la carte ne
  dépend du contrôleur tactile GT911, tu peux les réutiliser comme GPIO
  génériques (boutons, capteurs, UART vers instruments NMEA0183...).
- **+4 broches récupérables si tu n'utilises pas la carte microSD** :
  GPIO10-13. À garder si tu veux logger des données de navigation,
  sinon libres aussi.
- **+2 broches techniquement libres mais à garder si possible** :
  GPIO43/44 (UART debug) — utile pour le monitor série en développement,
  à ne sacrifier qu'en dernier recours.

**Total réaliste : 5 GPIO libres si tu gardes la carte SD (17, 18, 19,
20, 38), jusqu'à 9 si tu abandonnes aussi la carte SD**, sans toucher au
port debug UART.
