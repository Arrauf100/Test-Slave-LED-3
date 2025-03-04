/*

Test Slave Description :
- Test Slave 3 Led Matrix connect with Test-Master-3
- Matrix 3 X 32 X 64 P5

Baut Rate (9600)
- Ping: Mengecek apakah slave merespons dengan (PING,1), (PING,2), (PING,3)
- Clear: Menghapus tampilan slave dengan (CLEAR,1), (CLEAR,2), (CLEAR,3)
- Brightness: Mengatur tingkat kecerahan (BRIGHT,1,50), (BRIGHT,2,75), (BRIGHT,3,100)
- Parsing Command Otomatis: Bisa mengirim beberapa perintah sekaligus

*/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Arduino.h>
#include <iostream>
#include "matrixMod.h"

// Konfigurasi pin
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 21
#define G2_PIN 22
#define B2_PIN 23
#define A_PIN 12
#define B_PIN 16
#define C_PIN 17
#define D_PIN 18
#define LAT_PIN 32
#define OE_PIN 33
#define CLK_PIN 15

#define PANEL_RES_X 128 // Resolusi horizontal
#define PANEL_RES_Y 16  // Resolusi vertikal
#define PANEL_CHAIN 3   // Jumlah panel yang terhubung

#define DRIVER ICN2038S;

int center1 = 0;
int center2 = 0;
int center3 = 0;

int textX;
int textY;

String value1;
String value2;
String value3;

HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, -1, LAT_PIN, OE_PIN, CLK_PIN};

// MatrixPanel_I2S_DMA *dma_display;

void setup()
{
    Serial.begin(9600);
    Serial.println("Slave Ready...");

    HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);
    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S;

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->fillScreen(0);
    dma_display->setBrightness8(255); // 0-255
    dma_display->clearScreen();
}

void loop()
{
    if (Serial.available())
    {
        String received = Serial.readStringUntil('\n'); // Baca data dari Master
        received.trim();                                // Hapus spasi dan karakter tak terlihat

        if (received.startsWith("(") && received.endsWith(")"))
        {
            Serial.println("ACK: " + received); // Kirim balasan ke Master

            if (received == "(ping)")
            {
                Serial.println("Modul Matrix Active");
            }
            if (received == "(clear)")
            {
                dma_display->clearScreen();
                Serial.println("{ \"clear\" : true }");
            }
            if (received.startsWith("(B,"))
            {
                String brightnessValue = received.substring(3, received.length() - 1);
                dma_display->setBrightness8(brightnessValue.toInt());
                Serial.println("{ \"Brightness\" : " + brightnessValue + " }");
            }
            if (received.startsWith("(1,"))
            {
                dma_display->fillScreen(dma_display->color565(0, 0, 0));
                center1 = 0;
                value1 = received.substring(3, received.length() - 1);
                center1 = (30 / 2) - ((value1.length() * 6) / 2);
                displayText(center1 + 30, 4, value1.c_str(), 3);
                Serial.println("{ \"led\" : true }");
            }

            if (received.startsWith("(2,"))
            {

                dma_display->fillScreen(dma_display->color565(0, 0, 0));
                center2 = 0;
                drawText10x14(4, 8, "MATERIAL:");
                // displayText(4, 8, "MATERIAL:", 2);
                value2 = received.substring(3, received.length() - 1);
                center2 = (30 / 2) - ((value2.length() * 6) / 2);
                displayText(122, 4, value2.c_str(), 2);
                Serial.println("{ \"led\" : true }");
            }

            if (received.startsWith("(3,"))
            {

                dma_display->fillScreen(dma_display->color565(0, 0, 0));
                center3 = 0;
                displayText(4, 8, "DUMPING : ", 2);
                value3 = received.substring(3, received.length() - 1);
                center3 = (30 / 2) - ((value2.length() * 6) / 2);
                displayText(130, 8, value3.c_str(), 2);
                Serial.println("{ \"led\" : true }");
            }
            else
            {
                Serial.println("{ \"Error\" : \"Unknown command\" }");
            }
        }
        else
        {
            Serial.println("{ \"Error\" : \"Invalid command format\" }");
        }
    }
}
