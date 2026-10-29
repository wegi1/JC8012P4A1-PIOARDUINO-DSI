
#include <Arduino.h>
#pragma GCC push_options
#pragma GCC optimize("O3")

#include "pins_config.h"
#include "jd9365_lcd.h"
#include "gsl3680_touch.h"

// Obiekty globalne z wegi1 (inicjalizują bazowe piny zasilania i BL)
jd9365_lcd lcd = jd9365_lcd(LCD_RST);
gsl3680_touch touch = gsl3680_touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

// POPRAWKA: Bufory 16-bitowe (RGB565), dokładnie tak jak w natywnym potoku graficznym
uint16_t *buffer_red = nullptr;
uint16_t *buffer_blue = nullptr;

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=================================================");
    Serial.println(">>> SZYBKI MIGACZ RGB565 (BEZ LVGL) - REWIZJA v5 <<<");
    Serial.println("=================================================");

    // 1. BUDZENIE SPRZĘTU (Zasilanie, zegary DSI, Backlight)
    // Robimy to NAJPIERW, aby ustabilizować kontroler DMA
    lcd.begin();
    touch.begin();
    Serial.println("[HARDWARE] Magistrala MIPI-DSI zainicjalizowana.");

    // 2. NOWA ALOKACJA: 16-bit (2 bajty na piksel)
    uint32_t total_pixels = LCD_H_RES * LCD_V_RES;
    uint32_t total_bytes = total_pixels * 2; // Zmiana z 4 na 2 bajty!

    Serial.printf("[PSRAM] Rezerwacja odchudzonych buforów: 2x %d bajtów...\n", total_bytes);
    buffer_red = (uint16_t *)heap_caps_malloc(total_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    buffer_blue = (uint16_t *)heap_caps_malloc(total_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (buffer_red == nullptr || buffer_blue == nullptr)
    {
        Serial.println("CRITICAL: Brak pamięci w PSRAM dla RGB565!");
        while (1)
            ;
    }

    // 3. DEFINICJA KOLORÓW W FORMACIE RGB565 (16-bit: RRRRRGGGGGGBBBBB)
    // Czerwony: R=max (0x1F), G=0, B=0       -> 0xF800
    // Niebieski: R=0, G=0, B=max (0x1F)      -> 0x001F
    for (uint32_t i = 0; i < total_pixels; i++)
    {
        buffer_red[i] = 0xF800;
        buffer_blue[i] = 0x001F;
    }
    Serial.println("[PSRAM] Bufory RGB565 zostały przygotowane w pamięci.");
    Serial.println(">>> START PĘTLI GŁÓWNEJ <<<\n");
}

void loop()
{
    Serial.println("[LOG] Wysyłanie DMA -> CZERWONY");
    // Przekazujemy adres bufora rzutowany na (uint8_t*) zgodnie z sygnaturą funkcji
    lcd.lcd_draw_bitmap(0, 0, LCD_H_RES, LCD_V_RES, (uint8_t *)buffer_red);
    delay(1000);

    Serial.println("[LOG] Wysyłanie DMA -> NIEBIESKI");
    lcd.lcd_draw_bitmap(0, 0, LCD_H_RES, LCD_V_RES, (uint8_t *)buffer_blue);
    delay(1000);
}
