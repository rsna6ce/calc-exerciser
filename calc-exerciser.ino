#include "esp_system.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include "tone_melody.h"

// generated https://github.com/rsna6ce/rgb8882rgb565
#include "img/opening_bgr.h"
#include "img/menu_bgr.h"
#include "img/background_bgr.h"
#include "img/playgame_bgr.h"
#include "img/result_bgr.h"
#include "img/12pt_0_bgr.h"
#include "img/12pt_1_bgr.h"
#include "img/12pt_2_bgr.h"
#include "img/12pt_3_bgr.h"
#include "img/12pt_4_bgr.h"
#include "img/12pt_5_bgr.h"
#include "img/12pt_6_bgr.h"
#include "img/12pt_7_bgr.h"
#include "img/12pt_8_bgr.h"
#include "img/12pt_9_bgr.h"
#include "img/12pt_star_bgr.h"
#include "img/18pt_0_bgr.h"
#include "img/18pt_1_bgr.h"
#include "img/18pt_2_bgr.h"
#include "img/18pt_3_bgr.h"
#include "img/18pt_4_bgr.h"
#include "img/18pt_5_bgr.h"
#include "img/18pt_6_bgr.h"
#include "img/18pt_7_bgr.h"
#include "img/18pt_8_bgr.h"
#include "img/18pt_9_bgr.h"
#include "img/18pt_add_bgr.h"
#include "img/18pt_div_bgr.h"
#include "img/18pt_eq_bgr.h"
#include "img/18pt_mul_bgr.h"
#include "img/18pt_qm_bgr.h"
#include "img/18pt_sub_bgr.h"

const uint32_t font12_width = bmp565_12pt_0_width;
const uint32_t font12_height = bmp565_12pt_0_height;
const uint16_t* font12_pixels[] = {
    &bmp565_12pt_0_pixels[0],
    &bmp565_12pt_1_pixels[0],
    &bmp565_12pt_2_pixels[0],
    &bmp565_12pt_3_pixels[0],
    &bmp565_12pt_4_pixels[0],
    &bmp565_12pt_5_pixels[0],
    &bmp565_12pt_6_pixels[0],
    &bmp565_12pt_7_pixels[0],
    &bmp565_12pt_8_pixels[0],
    &bmp565_12pt_9_pixels[0],
};
const uint32_t font18_width = bmp565_18pt_0_width;
const uint32_t font18_height = bmp565_18pt_0_height;
const uint16_t* font18_pixels[] = {
    &bmp565_18pt_0_pixels[0],
    &bmp565_18pt_1_pixels[0],
    &bmp565_18pt_2_pixels[0],
    &bmp565_18pt_3_pixels[0],
    &bmp565_18pt_4_pixels[0],
    &bmp565_18pt_5_pixels[0],
    &bmp565_18pt_6_pixels[0],
    &bmp565_18pt_7_pixels[0],
    &bmp565_18pt_8_pixels[0],
    &bmp565_18pt_9_pixels[0],
};

#define TFT_CS  26
#define TFT_DC  27
#define TFT_RST  5
#define TFT_MOSI 23  // Data out(SDA)
#define TFT_SCLK 18  // Clock out (SCL/SCK)
#define TFT_BL   19  // Backlight LED
#define RGB565_TO_BGR565(x) (((x)<<11) | ((x)&0x07e0) | ((x)>>11))
#define RGB_TO_BGR565(r,g,b) ((b)>>3 | (g)>>2 | (r)>>3)
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


const uint32_t buz_pin = 13;
const uint32_t buz_ch = 0;
ToneMelody tone_melody = ToneMelody(buz_ch, buz_pin);

// melody define chime
tone_t chime_tones[] = {
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
struct melody_t chaim_melody = TONES_TO_MELODY(chime_tones);

// melody define correct
tone_t correct_tones[] = {
    {tone_si2, delay0},
    {tone_so2, delay1}};
struct melody_t correct_melody = TONES_TO_MELODY(correct_tones);

// melody define incorect
tone_t incorrect_tones[] = {
    {tone_do, delay2}};
struct melody_t incorrect_melody = TONES_TO_MELODY(incorrect_tones);

// melody define silent
tone_t silent_tones[] = {
    {tone_no, delay0}};
struct melody_t silent_melody = TONES_TO_MELODY(silent_tones);

void setup() {
    Serial.begin(115200);
    Serial.println("calc exerciser!");

    tft.initR(INITR_GREENTAB);
    tft.setRotation(1); //90deg x 1
    //tft.fillScreen(ST77XX_BLACK);
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_opening_pixels[0], bmp565_opening_width, bmp565_opening_height);

    tone_melody.begin();
    tone_melody.play_tone_melody_async(&chaim_melody);

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
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_result_pixels[0], bmp565_result_width, bmp565_result_height);
    delay(2000);
}

uint8_t tenkey_pin_xyz[]  = {25,32,33};
uint8_t tenkey_pin_abcd[] = {34,35,36,39};
uint8_t tenkey_keymap[] = {10, 1, 4, 7, 0, 2, 5, 8, 11, 3, 6, 9};
uint8_t tenkey_button_status[12]={1,1,1,1,1,1,1,1,1,1,1,1};
void loop2(void * params) {
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
                    Serial.print(tenkey_keymap[index]);
                    Serial.println("");
                }
            }
        }
        delay(5);
    }
}

enum screen_id {
    screen_openning,
    screen_menu,
    screen_countdown,
    screen_playgame,
    screen_result,
};
enum screen_id curr_screen_id = screen_openning;

enum menu_id {
    menu_add1, menu_add2,
    menu_sub1, menu_sub2,
    menu_mul1, menu_mul2,
    menu_div1, menu_div2,
    menu_centinel};
uint32_t curr_menu_id = menu_add1;

uint32_t countdown_next_millis = 0;
uint32_t countdown_next_count = 0;
uint32_t playgame_next_millis = 0;
uint32_t playgame_time_remain = 0;
uint32_t playgeme_bar_padding = 3;
uint32_t playgame_bar_height = 14;

const uint32_t playgeme_time_limit = 60*1000;

void screen_keyevent_openning(uint8_t key_no, uint32_t curr_millis) {
    Serial.print("screen_keyevent_openning key ");
    Serial.println(key_no);
    tone_melody.play_tone_melody_async(&silent_melody);
    curr_screen_id = screen_menu;
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_menu_pixels[0], bmp565_menu_width, bmp565_menu_height);
    uint16_t y = ((137-37)/4 * (curr_menu_id/2)) + 37;
    uint16_t x = ((141-103) * (curr_menu_id&1)) + 103;
    tft.fillCircle(x, y, 3, RGB565_TO_BGR565(ST77XX_RED));
}

void screen_keyevent_menu(uint8_t key_no, uint32_t curr_millis) {
    Serial.print("screen_keyevent_menu key ");
    if (key_no == 0) {
        curr_menu_id++;
        if (curr_menu_id >= menu_centinel) {
            curr_menu_id = menu_add1;
        }
        // top-left(103,37), bottom-right(141,137)
        for (uint32_t i=0; i<menu_centinel; i++) {
            uint16_t y = ((137-37)/4 * (i/2)) + 37;
            uint16_t x = ((141-103) * (i&1)) + 103;
            if (i == curr_menu_id) {
                tft.fillCircle(x, y, 3, RGB565_TO_BGR565(ST77XX_RED));
            } else {
                tft.fillCircle(x, y, 3, ST77XX_WHITE);
            }
        }
    } else if (key_no == 11) {
        countdown_next_millis = curr_millis + 100;
        countdown_next_count = 3;
        curr_screen_id = screen_countdown;
        tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_background_pixels[0], bmp565_background_width, bmp565_background_height);
    }
}
void screen_keyevent_countdown(uint8_t key_no, uint32_t curr_millis) {
    Serial.print("screen_keyevent_countdown key ");
    Serial.println(key_no);
    // do nothing
}
void screen_keyevent_playgame(uint8_t key_no, uint32_t curr_millis) {
    Serial.print("screen_keyevent_playgame key ");
    Serial.println(key_no);
}
void screen_keyevent_result(uint8_t key_no, uint32_t curr_millis) {
    Serial.print("screen_keyevent_result key ");
    Serial.println(key_no);
}

void screen_timerevent_countdown(uint32_t curr_millis) {
    if (countdown_next_millis==0 || curr_millis < countdown_next_millis) {
        return;
    }
    Serial.print("screen_timerevent_countdown countdown_next_count ");
    Serial.println(countdown_next_count);
    uint16_t x = (SCREEN_WIDTH - font12_width) /2;
    uint16_t y = (SCREEN_HEIGHT - font12_height) /2;
    tft.drawRGBBitmap(x,y, (uint16_t*)font18_pixels[countdown_next_count], font18_width, font18_height);
    countdown_next_millis += 1000;
    if (countdown_next_count-- == 0) {
        countdown_next_millis = 0;
        curr_screen_id = screen_playgame;
        playgame_next_millis = curr_millis + 100;
        playgame_time_remain = playgeme_time_limit;
        tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_playgame_pixels[0], bmp565_playgame_width, bmp565_playgame_height);
        tft.fillRect(playgeme_bar_padding, playgeme_bar_padding, SCREEN_WIDTH-playgeme_bar_padding*2, playgame_bar_height, RGB565_TO_BGR565(ST77XX_BLUE));
    }
}
void screen_timerevent_playgame(uint32_t curr_millis) {
    
}


const uint32_t timerevent_interval = 10;
uint32_t prev_timerevent_millis = 0;
uint8_t prev_index = 0xff;
void loop(void) {
    uint32_t curr_millis = millis();
    uint8_t key_index = 0xff;
    for (int i=0; i<12; i++) {
        if (tenkey_button_status[i] == 0) {
            key_index = i;
            break;
        }
    }
    if (prev_index != key_index) {
        prev_index = key_index;
        if (key_index != 0xff) {
            uint8_t keyno = tenkey_keymap[key_index];
            if (curr_screen_id == screen_openning) {
                screen_keyevent_openning(keyno, curr_millis);
            } else if (curr_screen_id == screen_menu) {
                screen_keyevent_menu(keyno, curr_millis);
            } else if (curr_screen_id == screen_countdown) {
                screen_keyevent_countdown(keyno, curr_millis);
            } else if (curr_screen_id == screen_playgame) {
                screen_keyevent_playgame(keyno, curr_millis);
            } else if (curr_screen_id == screen_result) {
                screen_keyevent_result(keyno, curr_millis);
            }
        }
    }
    if (prev_timerevent_millis+timerevent_interval < curr_millis) {
        prev_timerevent_millis = curr_millis;
        if (curr_screen_id == screen_countdown) {
            screen_timerevent_countdown(curr_millis);
        } else if (curr_screen_id == screen_playgame) {
            screen_timerevent_playgame(curr_millis);
        }
    }
}
