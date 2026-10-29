# ESP32-P4 (ECO2) MIPI-DSI 10" Project

🌐 **Language / Język:** [English (Default)](#english-version) | [Wersja Polska](#wersja-polska)

---

<a name="english-version"></a>
## 🇬🇧 English Version

### 🚀 Status: Working bare-metal configuration for early devboard batch

This repository contains a 100% verified hardware configuration for a 10-inch high-resolution MIPI-DSI display, running on the early revision of the **ESP32-P4** chip. This codebase completely eliminates timing mismatch issues (such as the "periscope" / "diver mask" optical artifacts) and serves as an ultra-fast foundation for high-performance graphics interfaces.

### ⚠️ CRITICAL HARDWARE REVISION NOTE

This project has been developed and tested **exclusively for the early production batch (OLD BATCH)** of the Guition / Jingcai 10" development boards.
* **Processor:** ESP32-P4 Silicon Revision **v1.3 (ECO2)**.
* **Display:** 10" MIPI-DSI Panel (**800x1280**) driven by the **JD9365** controller (2 data lanes).
* **Touch Panel:** **GSL3680** chip.

> **IMPORTANT:** Newer ESP32-P4 silicon revisions (e.g., ECO7 / Rev 3.x) and updated Guition board revisions feature major architectural overhauls in the system bus, the DW-GDMA controller, and the DSI bridge initialization sequence. Trying to run stock drivers from the latest ESP-IDF on the old v1.3 silicon usually results in immediate AXI/AHB bus freezes or a black screen. **This repository bypasses those limitations.**

### 🛠️ Software Architecture & Environment

The project was successfully migrated from the Arduino IDE to **VSCode + PioArduino (PlatformIO)** and restructured for optimal code isolation:
* **Framework:** Stable `espressif32 @ 55.3.312` fork (aligned with the ESP-IDF v5.5.x line).
* **Directory Structure:**
  * `lib/lcd/` - Isolated, clean MIPI-DSI display driver (JD9365).
  * `lib/touch/` - Dedicated hardware abstraction layer for the touch panel (GSL3680).
  * `include/pins_config.h` - Fixed, verified pin mapping for the OLD batch hardware.
  * `src/main.cpp` - Application entry point with explicit `#include <Arduino.h>`.
* **Build Optimizations:**
  * Enforced `-O3` compiler flag for maximum optimization.
  * Disabled USB CDC at boot to avoid system bus overhead.
  * PlatformIO package storage relocated to `D:\` drive to completely bypass Windows path length limits (`MAX_PATH`).

### 🏎️ Graphical Strategy: Bare-Metal Scanlines

To squeeze out maximum fluidity (60 FPS) and avoid the memory/processing overhead of heavy frameworks (like LVGL), the project implements a custom raster-synchronization mechanism:
1. **Direct Register Access:** We monitor the physical DPI status register address in the MIPI-DSI bridge: `0x50120040`.
2. **Race the Raster:** Reading the upper 16 bits (`status >> 16`) directly yields the currently rendering screen line (scanline).
3. **Hardware Page Flipping:** Utilizing double buffering allocated in PSRAM. Vector graphics are rendered entirely in the hidden backbuffer. The loop queries the register, waits for the safe V-Blank phase (line > 1280 or past 3/4 of the screen), and instantly swaps the frame pointer at the hardware level, avoiding any blocking or heavy `memcpy()` operations.

### 🎯 Upcoming Roadmap Objectives

1. **Vector Font Integration:** Incorporating the `stb_truetype.h` single-header library and optimizing an 8-bit glyph alpha blending function directly into our 16-bit RGB565 framebuffer.
2. **Vector Clock Engine:** Positioning and rendering large, anti-aliased, ultra-smooth digits drawn directly from Flash memory.
3. **Native ESP-IDF Port:** Transferring this rock-solid, working initialization sequence into a pure **ESP-IDF v5.5.3** project via the official VSCode extension (`components/` architecture and CMake) to permanently secure system bus stability.

---

<a name="wersja-polska"></a>
## 🇵🇱 Wersja Polska

### 🚀 Status: Działająca konfiguracja bare-metal dla starszej rewizji płytki

Repozytorium zawiera w 100% sprawną, zweryfikowaną sprzętowo konfigurację dla 10-calowego panelu MIPI-DSI o wysokiej rozdzielczości, uruchomioną na wczesnej rewizji układu **ESP32-P4**. Kod eliminuje problem rozjechanych timingów (efekt "maski nurka" / "peryskopu") i stanowi idealną bazę pod ultra-wydajny interfejs graficzny.

### ⚠️ KRYTYCZNA UWAGA SPRZĘTOWA (REWIZJA UKŁADU)

Ten projekt został stworzony i przetestowany **wyłącznie dla wczesnej partii produkcyjnej (OLD BATCH)** płytek deweloperskich Guition / Jingcai 10". 
* **Procesor:** ESP32-P4 Silicon Revision **v1.3 (ECO2)**.
* **Ekran:** Matryca 10" MIPI-DSI (**800x1280**) sterowana przez kontroler **JD9365** (2 linie danych).
* **Panel Dotykowy:** Układ **GSL3680**.

> **WAŻNE:** Nowsze rewizje procesora ESP32-P4 (np. ECO7 / Rev 3.x) oraz nowsze wersje płytek Guition posiadają drastycznie zmienioną architekturę szyny systemowej, kontrolera DW-GDMA oraz inne parametry inicjalizacji mostka DSI. Próba uruchomienia standardowych bibliotek z nowego ESP-IDF na starym krzemie v1.3 powoduje natychmiastowe zamrożenie szyny (freeze) lub brak obrazu. **To repozytorium rozwiązuje ten problem.**

### 🛠️ Architektura Programowa & Środowisko

Projekt został zmigrowany z Arduino IDE do **VSCode + PioArduino (PlatformIO)** i zrestrukturyzowany w celu zapewnienia maksymalnej czystości kodu:
* **Framework:** Stabilny fork `espressif32 @ 55.3.312` (zgodny z linią ESP-IDF v5.5.x).
* **Struktura katalogów:**
  * `lib/lcd/` - Odizolowany, czysty sterownik ekranu MIPI-DSI (JD9365).
  * `lib/touch/` - Wydzielona obsługa panelu dotykowego (GSL3680).
  * `include/pins_config.h` - Zabetonowana, poprawna konfiguracja pinów dla wersji OLD.
  * `src/main.cpp` - Główny program z jawnym `#include <Arduino.h>`.
* **Optymalizacje systemowe:**
  * Wymuszona kompilacja z flagą `-O3` (Maximum Optimization).
  * Zablokowane USB CDC na starcie dla odciążenia peryferiów.
  * Przeniesione bazy pakietów PlatformIO na dysk `D:\` w celu ominięcia limitu ścieżek Windows (`MAX_PATH`).

### 🏎️ Strategia Graficzna: Bare-Metal Scanlines

W celu wyciśnięcia maksymalnej płynności (60 FPS) i ominięcia narzutu ciężkich frameworków (np. LVGL), projekt implementuje autorski mechanizm synchronizacji z rastrem ekranu:
1. **Bezpośredni odczyt rejestru:** Monitorujemy fizyczny adres statusu DPI w mostku MIPI-DSI: `0x50120040`.
2. **Wyścig z rastrem:** Odczyt górnych 16 bitów (`status >> 16`) zwraca numer aktualnie renderowanej linii (scanline).
3. **Hardware Page Flipping:** Wykorzystujemy podwójne buforowanie w pamięci PSRAM. Rysowanie wektorowe odbywa się w ukrytym buforze (backbuffer). Pętla oczekuje na bezpieczną fazę V-Blank (linia > 1280 lub 3/4 ekranu), po czym następuje natychmiastowa, sprzętowa podmiana wskaźnika klatki bez blokującego i zasobożernego `memcpy()`.

### 🎯 Cele na najbliższy etap rozwoju

1. **Integracja czcionek wektorowych:** Wdrożenie biblioteki `stb_truetype.h` (single-header) oraz optymalizacja renderowania 8-bitowej maski glifów z antyaliasingiem (Alpha Blending) bezpośrednio do 16-bitowego bufora RGB565.
2. **Wektorowy Zegar:** Budowa zegara o idealnie gładkich krawędziach cyfr, renderowanych wprost z pamięci Flash.
3. **Czysty Port ESP-IDF:** Przeniesienie sprawdzonej sekwencji startowej panelu z PioArduino do czystego środowiska **ESP-IDF v5.5.3** za pomocą oficjalnego rozszerzenia VSCode (architektura `components/` i CMake), aby trwale zabezpieczyć stabilność szyny systemowej.
