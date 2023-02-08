# Projekt Hinfosvc

Ohodnotený na 17/20 bodov. 2 body boli odobrané za to, že to nešlo na testovacom stroji preložiť na prvý pokus. To je už opravené a dá sa to preložiť. Inak program prešiel všetkými testami. Dokumentácia mohla byť lepšia.

Utilita Hinfosvc umožnuje jednoducho získavať informácie o serveri, na ktorom beží. štatistiky možno získať cez webový prehliadač alebo v termináli. Funkcionalita zahŕňa zisťovanie hostname servera, názvu procesora a aktuálneho zaťaženia procesora.

## Súborová štruktúra

Na úspešné skompilovanie projektu sú nutné štyri súbory: hinfosvc.c, str_obj.c, str_obj.h a Makefile.

### Hinfosvc

Tento súbor obsahuje zdrojový kód hlavnej časti programu. Obsluhuje sieťové spojenie a získava systémové parametre. Parsuje prichádzajúce správy a na základe toho odosiela odpoveď. Typ odpoveďe zisťuje na základe toho, čo je za GET v prichádzajúcej správe.

#### Získavanie parametrov

Meno servera sa získava zo súboru /etc/hostname. Názov procesora sa získava zo súboru /proc/cpuinfo. Zaťaženie procesora sa získava zo súboru /proc/stat. Zo sekundovým rozdielom sa z neho vyčitajú údaje a na základe ich rozdielov sa dopočíta zaťaženie. Parametre sa z daných súborov predspracujú a vyfiltrujú pomocou systémových utilít Linuxu. Program tak načitava do svojej pamäťe už predspracované dáta.

### Str_obj

Knižnica na podporu práce s reťazcovými premennými. Umožnuje pracovať s reťazcami ako s premennou s dynamickou veľkosťou. Implementuje funkcie na prehľadávanie textu, jeho prechádzanie, spájenie reťazcov a posun o slová. Ǔlohou tejto knižnice je abstrahovať prácu s reťazcami. Používa to hlavne obsluha prichádzajúceho, paketu keďže jeho veľkosť je dopredu neznáma.

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

* <http://server:port/hostname>
* <http://server:port/cpu-name>
* <http://server:port/load>

Hostname vráti sieťové meno počítača. Cpu-name vráti meno procesora počítača. Load vráti aktuálne zaťaženie procesora v %.

Autor: Juraj Novosád  
email: xnovos13@stud.fit.vutbr.cz
