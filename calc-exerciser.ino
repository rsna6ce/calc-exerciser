#include "esp_system.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include "tone_melody.h"

#include "img/opening_bgr.h"
#include "img/menu_bgr.h"
#include "img/background_bgr.h"
#include "img/playgame_bgr.h"
#include "img/finish_bgr.h"

#define TFT_CS  26
#define TFT_DC  27
#define TFT_RST  5
#define TFT_MOSI 23  // Data out(SDA)
#define TFT_SCLK 18  // Clock out (SCL/SCK)
#define TFT_BL   19  // Backlight LED

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
float p = 3.1415926;

const uint32_t buz_pin = 13;
const uint32_t buz_ch = 0;

ToneMelody tone_melody = ToneMelody(buz_ch, buz_pin);

// chaim
tone_t chaim_tones[] = {
    {tone_fa, delay1},
    {tone_ra, delay1},
    {tone_so, delay1},
    {tone_do, delay2},
    {tone_no, delay0},
    {tone_do, delay1},
    {tone_so, delay1},
    {tone_ra, delay1},
    {tone_fa, delay2},
    {tone_no, delay0},
    {tone_ra, delay1},
    {tone_fa, delay1},
    {tone_so, delay1},
    {tone_do, delay2},
    {tone_no, delay0},
    {tone_do, delay1},
    {tone_so, delay1},
    {tone_ra, delay1},
    {tone_fa, delay2}};
struct melody_t chaim_melody = TONES_TO_MELODY(chaim_tones);

// correct
tone_t correct_tones[] = {
    {tone_si2, delay0},
    {tone_so2, delay1}};
struct melody_t correct_melody = TONES_TO_MELODY(correct_tones);

// incorect
tone_t incorrect_tones[] = {
    {tone_do, delay2}};
struct melody_t incorrect_melody = TONES_TO_MELODY(incorrect_tones);

void setup() {
    Serial.begin(115200);
    Serial.println("calc exerciser!");

    //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
    tft.initR(INITR_GREENTAB);
    tft.setRotation(1); //90deg x 1
    uint16_t time = millis();
    tft.fillScreen(ST77XX_BLACK);
    time = millis() - time;
  
      // tft print function!
    tftPrintTest();
    delay(4000);

    tone_melody.begin();

    xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 2, NULL, 0);
}

void tftPrintTest() {
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_opening_pixels[0], bmp565_opening_width, bmp565_opening_height);
    delay(2000);
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_menu_pixels[0], bmp565_menu_width, bmp565_menu_height);
    delay(2000);
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_background_pixels[0], bmp565_background_width, bmp565_background_height);
    delay(2000);
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_playgame_pixels[0], bmp565_playgame_width, bmp565_playgame_height);
    delay(2000);
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_finish_pixels[0], bmp565_finish_width, bmp565_finish_height);
    delay(2000);
}

uint8_t tenkey_pin_xyz[]  = {25,32,33};
uint8_t tenkey_pin_abcd[] = {34,35,36,39};
uint8_t tenkey_index2keyno[] = {10, 7, 4, 1, 0, 8, 5, 2, 11, 9, 6, 3};
uint8_t tenkey_keyno2index[] = {4, 3, 7, 11, 2, 6, 10, 1, 5, 9, 0, 8};
uint8_t tenkey_button_status[12]={};
void loop2(void * params) {
    Serial.println("loop2");
    for (uint32_t i=0; i<sizeof(tenkey_pin_xyz); i++) {
        pinMode(tenkey_pin_xyz[i], OUTPUT);
    }
    for (uint32_t i=0; i<sizeof(tenkey_pin_abcd); i++) {
        pinMode(tenkey_pin_abcd[i], INPUT);
    }
    while(true) {
        for (uint32_t x=0; x<3; x++) {
            digitalWrite(tenkey_pin_xyz[0], (x!=0));
            digitalWrite(tenkey_pin_xyz[1], (x!=1));
            digitalWrite(tenkey_pin_xyz[2], (x!=2));
            for (uint32_t a=0; a<4; a++) {
                uint32_t index = x*4 + a;
                uint8_t val = digitalRead(tenkey_pin_abcd[a]);
                if (tenkey_button_status[index] != val) {
                    tenkey_button_status[index] = val;
                    Serial.print("index:");
                    Serial.print(index);
                    Serial.print(" val:");
                    Serial.print(val);
                    Serial.print(" key:");
                    Serial.print(tenkey_index2keyno[index]);
                    Serial.println("");
                }
            }
        }
        delay(5);
    }
}

uint32_t key2freq[] = {
tone_fa2,
tone_do , tone_re , tone_mi ,
tone_fa , tone_so , tone_ra ,
tone_si , tone_do2, tone_re2,
tone_mi2, tone_so2,
};

int temp = 0;
int prev_index = -1;
void loop(void) {
    if (digitalRead(0)) {
        int index = -1;
        for (int i=0; i<12; i++) {
            if (tenkey_button_status[i] == 0) {
                index = i;
                break;
            }
        }
        if (prev_index != index) {
            prev_index = index;
            if (index >= 0) {
                ledcWriteTone(buz_ch, key2freq[ tenkey_index2keyno[index]]);
            } else {
                ledcWriteTone(buz_ch, tone_no);
            }
        }
    } else {
        if (temp == 0) {
            tone_melody.play_tone_melody_async(&chaim_melody);
            temp++;
        } else if (temp == 1) {
            tone_melody.play_tone_melody_async(&correct_melody);
            temp++;
        } else if (temp == 2) {
            tone_melody.play_tone_melody_async(&incorrect_melody);
            temp++;
        } else {
            temp = 0;
        }
        delay(500);
    }
}
