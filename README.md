# ESP32 Laserski radar

Radar izrađen na ESP32 mikrokontroleru koji pomoću **VL53L1X laserskog senzora udaljenosti** postavljenog na **servo motor** skenira prostor ispred sebe i prikazuje udaljenosti objekata u obliku klasičnog radarskog prikaza na **2.4" TFT zaslonu osjetljivom na dodir**.

Servo okreće senzor kroz luk od 180°, senzor mjeri udaljenost u svakom položaju, a izmjerene točke se u stvarnom vremenu ucrtavaju na zaslon u obliku radarskog grafa.

## Značajke

- Skeniranje u rasponu od 180° (od -90° do +90°)
- Radarski prikaz s lukovima udaljenosti (50, 100, 150 i 200 cm) i kutnim oznakama (-90°, -45°, 0°, 45°, 90°)
- Animirana linija skeniranja koja prati položaj servo motora
- Detektirani objekti iscrtavaju se kao obojeni isječci: **crveno** za objekte bliže od 150 cm, **žuto** za udaljenije
- Alternativni prikaz **mjerača udaljenosti** (poput dB metra)
- Sučelje na dodir s gumbima **START/STOP** (pauziranje i nastavak skeniranja) i **DISPLAY** (prebacivanje između radarskog prikaza i mjerača udaljenosti)
- Domet prikaza do 200 cm

## Korišteni dijelovi

| Komponenta | Opis |
|---|---|
| [Soldered Dasduino CONNECTPLUS](https://soldered.com/products/nula-deepsleep-esp32-s3-board) | Mikrokontroler |
| [Soldered TFT LCD Breakout 2.4" with Touch](https://docs.soldered.com/tft-lcd/overview/) | Zaslon 320x240 (ILI9341) sa Touchscreen-om |
| [Soldered VL53L1X Breakout](https://soldered.com/products/vl53l1x-tof-laser-distance-sensor) | Laserski ToF senzor udaljenosti (I2C) |
| Servo motor | Standardni servo za okretanje senzora |

## Spajanje

| Funkcija | ESP32 pin |
|---|---|
| Servo (signal) | GPIO 15 |
| TFT SCK (VSPI) | GPIO 18 |
| TFT MISO (VSPI) | GPIO 19 |
| TFT MOSI (VSPI) | GPIO 23 |
| TFT CS | GPIO 25 |
| TFT DC | GPIO 33 |
| TFT RST | nije spojen (`-1`) |
| Touch CS | GPIO 5 |
| VL53L1X (SDA / SCL) | zadani I2C pinovi ESP32 (GPIO 21 / GPIO 22) |

> Servo se preporučuje napajati iz zasebnog izvora napajanja od 5 V (sa zajedničkim GND), a ne izravno s ESP32 pina, kako bi napajanje bilo stabilno bez skokova.

## Struktura projekta

```
.
├── main_sketch.ino   # Glavni program: setup, petlja skeniranja, obrada dodira
├── Components.h/.cpp # Klasa Components: upravljanje servom i čitanje VL53L1X senzora
└── GUI.h/.cpp        # Klasa GUI: crtanje radara, mjerača udaljenosti i gumba
```

### `main_sketch.ino`
Inicijalizira komponente i sučelje te u glavnoj petlji svakih 50 ms pomiče servo, očitava udaljenost i ažurira zaslon. Također obrađuje dodire na zaslonu i mijenja stanje sustava (START/STOP) i način prikaza.

### `Components`
- `begin()` – inicijalizira servo i VL53L1X senzor u *Long* načinu rada te pokreće kontinuirano mjerenje
- `getDistance()` – vraća udaljenost u centimetrima (senzor mjeri u mm) i ispisuje upozorenje na serijski monitor u slučaju timeouta
- `setServoAngle(angle)` – postavlja kut servo motora

### `GUI`
- `begin()` – inicijalizira VSPI sabirnicu (40 MHz), zaslon i dodir te crta početni prikaz
- `drawRadarGraph()` / `redrawRadar()` – iscrtavanje polukružne mreže radara s oznakama
- `updateSweepLine()` – crtanje i brisanje linije skeniranja
- `drawDetectedObject()` / `eraseDetectedObject()` – crtanje i brisanje detektiranih objekata
- `drawDistanceMeter()` – prikaz udaljenosti kao segmentirani mjerač
- `drawButtons()` – crtanje gumba START/STOP i DISPLAY

## Biblioteke

Za prevođenje projekta potrebno je instalirati:

- `ESP32_Servo` (ESP32Servo)
- `VL53L1X-SOLDERED` (Soldered biblioteka za VL53L1X)
- `TFT-LCD-Breakout-2.4-With-Touch-SOLDERED` (Soldered biblioteka za TFT zaslon)
- `Adafruit GFX Library`
- `SPI` (dolazi uz ESP32 jezgru)
