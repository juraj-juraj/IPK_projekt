# Projekt Hinfosvc

Utilita Hinfosvc umožnuje jednoducho získavať informácie o serveri, na ktorom beží. štatistiky možno získať cez webový prehliadač alebo v termináli. Funkcionalita zahŕňa zisťovanie hostname servera, názvu procesora a aktuálneho zaťaženie procesora.

## Súborová štruktúra

Na úspešné skompilovanie projektu sú nutné štyri súbory: hinfosvc.c, str_obj.c, str_obj.h a Makefile.

### Hinfosvc

Tento súbor obsahuje zdrojový kód hlavnej časti programu. Obsluhuje sieťové spojenie a získava systémové parametre. Parsuje prichádzajúce správy a na základe toho odosiela odpoveď. Typ odpoveďe zisťuje na základe toho, čo je za GET v prichádzajúcej správe.

#### Získavanie parametrov

Meno servera sa získava zo súboru /etc/hostname. Názov procesora sa získava zo súboru /proc/cpuinfo. Zaťaženie procesora sa získava zo súboru /proc/stat. Zo sekundovým rozdielom sa z neho vyčitajú údaje a na základe ich rozdielov sa dopočita zaťaženie.

### Str_obj

Knižnica na podporu práce s reťazcovými premennými. Umožnuje pracovať s reťazcami ako s premennou s dynamickou veľkosťou. Implementuje funkcie na prehľadávanie textu, jeho prechádzanie, spájenie reťazcov.

## Kompilácia

Na skompilovanie projektu stači zadať v koreňovom adresári príkaz:

```bash
make
```

Na odstránenie vytvorených súborov existuje príkaz:

```bash
make clean
```

## Spustenie

Server potrebuje pri spustení parameter zadávajúci na akom porte má reagovať. Ako prvý argument je číslo s tým portom.

```bash
./hinfosvc 10800
```

## Požívanie

Po spustení servera reaguje na požiadavky:

* <http://server:10800/hostname>
* <http://server:10800/cpu-info>
* <http://server:10800/load>
