# Super Farmár – Semestrálna Práca

Tento projekt predstavuje textovú multiplayer hru "Super Farmár", vyvinutú v jazyku C ako semestrálnu prácu tímu.
Hra využíva klient-server architektúru a implementuje mechanizmy komunikácie prostredníctvom systémových rúr (pipes)
a zdieľanej pamäte.

Obsah:
- Popis Hry
- Pravidlá a Herné Mechaniky
- Inštalácia a Kompilácia
- Použitie
- Technológie
- Tím

--------------------------------------------------
Popis Hry:

Super Farmár je textovo založená hra, v ktorej sa hráči snažia vybudovať úspešnú farmu.
Hráči začínajú s jedným králikom a ich cieľom je získať aspoň jedno každé základné zviera (králik, ovca, krava, kôň).
Hra ponúka interaktívne príkazy, ktoré umožňujú hádzať kockou, meniť zvieratá v obchode, sledovať stav inventára a
riadiť priebeh ťahov.

--------------------------------------------------
Pravidlá a Herné Mechaniky:

- Cieľ hry:
  Získať aspoň jedno každé základné zviera (králik, ovca, krava, kôň) na farme.

- Štart:
  Každý hráč začína s jedným králikom.

- Hod kockou:
  - Ak padne rovnaké zviera na oboch kockách, zviera sa pripojí k tvojej farme.
  - Buď opatrný na symboly líšky a vlka, ktoré môžu ohroziť tvoju farmu (napr. líška môže zožrať všetky králiky,
    vlk ovce, prasa alebo kravy).

- Obchod a výmena:
  - Hráči môžu vymieňať zvieratá. Napríklad: 1 ovca za malého psa (ochranný prostriedok proti líške)
    a 1 krava za veľkého psa (ochrana pred vlkom).
  - V obchode nájdeš aj informácie o cenách a dostupnom inventári.

- Bonus:
  Za každý pár rovnakých zvierat získate na konci ťahu jedno ďalšie zviera.

--------------------------------------------------
Inštalácia a Kompilácia:

Projekt je postavený pomocou CMake. Pre zostavenie projektu postupuj nasledovne:

1. Klonovanie repozitára:
   git clone https://github.com/milanmazgut/PosSemestralnaPraca.git

2. Prechod do adresára projektu:
   cd PosSemestralnaPraca

3. Vytvorenie build adresára a prechod doň:
   mkdir build && cd build

4. Generovanie Makefile pomocou CMake:
   cmake ..

5. Zostavenie projektu:
   make

--------------------------------------------------
Použitie:

Projekt je spustiteľný v dvoch módoch – server a klient.

Spustenie Servera:
  Server sa spúšťa so špecifikáciou počtu požadovaných hráčov.
  Príkaz:
      ./program server <počet_hráčov>
  Príklad:
      ./program server 2

Spustenie Klienta:
  Klient sa spúšťa s menom hráča.
  Príkaz:
      ./program client <meno>
  Príklad:
      ./program client Marek

Dostupné Príkazy v Hre:
  - roll
      Hod kockou a pokus o pripojenie nového zvieraťa k farme.
  - exchange <zviera1> <zviera2>
      Vykonanie výmeny zvierat (napr. exchange ovca maly_pes).
  - inventory
      Zobrazenie inventára vlastných alebo iných hráčov.
  - shop prices
      Zobrazenie cien výmeny zvierat v obchode.
  - shop inventory
      Zobrazenie dostupných zvierat v obchode.
  - end
      Ukončenie aktuálneho ťahu.
  - quit
      Odpojenie hráča zo hry.
  - shutdown
      Ukončenie hry (administrátorský príkaz).

--------------------------------------------------
Technológie:

- Programovací jazyk: C (štandard C11)
- Komunikácia:
    - Systémové rúrky (pipes)
    - Zdieľaná pamäť (shared memory)
- Paralelizmus: Vlákna (pthread)
- Build systém: CMake

--------------------------------------------------
Tím:

Projekt vytvoril tím:
- Milan Mažgút
- Jakub Šimurka
- Matej Halama

--------------------------------------------------
Poznámky:

- Hra funguje v termináli a je určená pre operačné systémy podporujúce POSIX API.
- Uistite sa, že máte nainštalované potrebné nástroje (CMake, gcc/clang, pthread).
- Projekt bol vyvinutý ako semestrálna práca a môže obsahovať určité nedokonalosti či oblasti na ďalšie vylepšenie.

