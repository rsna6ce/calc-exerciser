#include "esp_system.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
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

#define TOSTR(x) #x
#define TOSTRX(x) TOSTR(x)
#define DEBUG_PRINT_VARIABLE(x) Serial.print(TOSTRX(x));Serial.print(":");Serial.println(x);

#define TFT_CS  26
#define TFT_DC  27
#define TFT_RST  5
#define TFT_MOSI 23  // Data out(SDA)
#define TFT_SCLK 18  // Clock out (SCL/SCK)
#define TFT_BL   19  // Backlight LED
#define RGB565_TO_BGR565(x) (((x)<<11) | ((x)&0x07e0) | ((x)>>11))
#define RGB_TO_BGR565(r,g,b) (((b)>>3)<<11 | ((g)>>2)<<5 | (r)>>3)
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

// melody define fanfare
tone_t fanfare_tones[] = {
    {tone_so,   delay0},
    {tone_do2,  delay0},
    {tone_mi2,  delay0},
    {tone_so2,  delay0},
    {tone_no,   delay0},
    {tone_mi2,  delay0},
    {tone_so2,  delay2}};
struct melody_t fanfare_melody = TONES_TO_MELODY(fanfare_tones);

// melody define oneup
tone_t oneup_tones[] = {
    {tone_mi2,   delay0},
    {tone_do3,   delay0},
    {tone_mi3,   delay0},
    {tone_do3,   delay0},
    {tone_re3,   delay0},
    {tone_so3,   delay0}};
struct melody_t oneup_melody = TONES_TO_MELODY(oneup_tones);

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

// playgame
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

uint32_t playgame_highsrore[menu_centinel] = {};
void write_highscore() {
    File fp = SPIFFS.open("/highscore.txt","w");
    if (fp) {
        for (uint32_t i=0; i<(uint32_t)menu_centinel; i++) {
            fp.print(String(playgame_highsrore[i]) + "\n");
        }
        fp.close();
    } else {
        Serial.println("ERROR: write_highscore failed.");
    }
}
void read_highscore() {
    File fp = SPIFFS.open("/highscore.txt","r");
    if (fp) {
        uint32_t index = 0;
        while (fp.available() && index<(uint32_t)menu_centinel) {
            String line = fp.readStringUntil('\n');
            playgame_highsrore[index] = line.toInt();
            index++;
        }
        fp.close();
    } else {
        Serial.println("ERROR: read_highscore failed.");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("calc exerciser!");
    //onboard sw
    pinMode(0, INPUT_PULLUP);

    tft.initR(INITR_GREENTAB);
    tft.setRotation(1); //90deg x 1
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_opening_pixels[0], bmp565_opening_width, bmp565_opening_height);

    tone_melody.begin();
    tone_melody.play_tone_melody_async(&chaim_melody);

    SPIFFS.begin(true);
    read_highscore();

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



uint32_t countdown_next_millis = 0;
uint32_t countdown_next_count = 0;
uint32_t playgame_next_millis = 0;
uint32_t playgame_start_millis = 0;
uint32_t playgeme_bar_padding = 3;
uint32_t playgame_bar_height = 12;
uint32_t playgame_input = 0;
uint32_t playgame_point = 0;
int32_t playgame_value1 = 0;
int32_t playgame_value2 = 0;
int32_t playgame_answer = 0;
const uint32_t playgeme_time_limit = 60*1000;

void screen_keyevent_openning(uint8_t key_no, uint32_t curr_millis) {
    if (digitalRead(0) == LOW) {
        // reset highscore
        memset( &playgame_highsrore[0], 0, sizeof(playgame_highsrore) );
        write_highscore();
    }

    tone_melody.play_tone_melody_async(&silent_melody);
    curr_screen_id = screen_menu;
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_menu_pixels[0], bmp565_menu_width, bmp565_menu_height);
    uint16_t y = ((137-37)/4 * (curr_menu_id/2)) + 37;
    uint16_t x = ((141-102) * (curr_menu_id&1)) + 102;
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
            uint16_t x = ((141-102) * (i&1)) + 102;
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
        randomSeed(curr_millis);
        tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_background_pixels[0], bmp565_background_width, bmp565_background_height);
    }
}


void gen_exercise_add(uint32_t level, int32_t* value1, int32_t* value2, int32_t* answer) {
    int32_t v1 = 0;
    int32_t v2 = 0;
    if (level == 1) {
        v1  = random(1, 9);
        v2  = random(1, 9);
    } else if (level == 2) {
        v1 = random(10, 99);
        v2 = random(1, 9);
    }
    *value1 = v1;
    *value2 = v2;
    *answer = v1 + v2;
}
void gen_exercise_sub(uint32_t level, int32_t* value1, int32_t* value2, int32_t* answer) {
    int32_t v1 = 0;
    int32_t v2 = 0;
    if (level == 1) {
        v1  = random(1, 9);
        v2  = random(1, v1);
    } else if (level == 2) {
        v1 = random(10, 99);
        v2 = random(1, 9);
    }
    *value1 = v1;
    *value2 = v2;
    *answer = v1 - v2;
}
void gen_exercise_mul(uint32_t level, int32_t* value1, int32_t* value2, int32_t* answer) {
    int32_t v1 = 0;
    int32_t v2 = 0;
    if (level == 1) {
        v1  = random(1, 9);
        v2  = random(1, 9);
    } else if (level == 2) {
        v1 = random(10, 99);
        v2 = random(1, 9);
    }
    *value1 = v1;
    *value2 = v2;
    *answer = v1 * v2;
}
void gen_exercise_div(uint32_t level, int32_t* value1, int32_t* value2, int32_t* answer) {
    int32_t v2 = 0;
    int32_t ans = 0;
    if (level == 1) {
        v2  = random(1, 9);
        ans  = random(1, 9);
    } else if (level == 2) {
        v2 = random(10, 99);
        ans = random(1, 9);
    }
    *value1 = v2 * ans;
    *value2 = v2;
    *answer = ans;
}
void gen_exercise(int32_t* value1, int32_t* value2, int32_t* answer) {
    int32_t temp_value1 = 0;
    int32_t temp_value2 = 0;
    int32_t temp_answer = 0;
    do {
        
        switch (curr_menu_id) {
            case menu_add1: gen_exercise_add(1, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_add2: gen_exercise_add(2, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_sub1: gen_exercise_sub(1, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_sub2: gen_exercise_sub(2, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_mul1: gen_exercise_mul(1, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_mul2: gen_exercise_mul(2, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_div1: gen_exercise_div(1, &temp_value1, &temp_value2, &temp_answer); break;
            case menu_div2: gen_exercise_div(2, &temp_value1, &temp_value2, &temp_answer); break;
        };
        // retry another exercise
    } while (temp_value1==*value1 && temp_value2==*value2);
    *value1 = temp_value1;
    *value2 = temp_value2;
    *answer = temp_answer;
}
void draw_string(uint16_t x, uint16_t y, uint32_t font_size, String s) {
    uint16_t curr_y = y;
    uint16_t curr_x = x;
    uint16_t char_width;
    uint16_t char_height;
    uint16_t **char_pixels;
    if (font_size == 12) {
        char_width = bmp565_12pt_0_width;
        char_height = bmp565_12pt_0_height;
        char_pixels = (uint16_t **)&font12_pixels[0];
    } else if(font_size == 18) {
        char_width = bmp565_18pt_0_width;
        char_height = bmp565_18pt_0_height;
        char_pixels = (uint16_t **)&font18_pixels[0];
    } else {
        return;
    }
    for (int i=0; i<s.length(); i++) {
        String chr = s.substring(i, i+1);
        if (chr == " ") {
            tft.fillRect(curr_x, curr_y, char_width, char_height, RGB565_TO_BGR565(ST77XX_WHITE));
            curr_x += char_width;
        } else if (chr == "0" || chr == "1" || chr == "2" || chr == "3" || chr == "4" ||
                   chr == "5" || chr == "6" || chr == "7" || chr == "8" || chr == "9") {
           int num = chr.toInt();
           tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)char_pixels[num], char_width, char_height);
           curr_x += char_width;
       } else if (chr == "+") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_add_pixels[0], bmp565_18pt_add_width, bmp565_18pt_add_height);
               curr_x += bmp565_18pt_add_width;
           }
       } else if (chr == "-") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_sub_pixels[0], bmp565_18pt_sub_width, bmp565_18pt_sub_height);
               curr_x += bmp565_18pt_sub_width;
           }
       } else if (chr == "*") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_mul_pixels[0], bmp565_18pt_mul_width, bmp565_18pt_mul_height);
               curr_x += bmp565_18pt_mul_width;
           }
       } else if (chr == "/") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_div_pixels[0], bmp565_18pt_div_width, bmp565_18pt_div_height);
               curr_x += bmp565_18pt_div_width;
           }
       } else if (chr == "=") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_eq_pixels[0], bmp565_18pt_eq_width, bmp565_18pt_eq_height);
               curr_x += bmp565_18pt_eq_width;
           }
       } else if (chr == "?") {
           if (font_size == 12) {
               // not impremented
           } else if (font_size == 18) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_18pt_qm_pixels[0], bmp565_18pt_qm_width, bmp565_18pt_qm_height);
               curr_x += bmp565_18pt_eq_width;
           }
       } else if (chr == "S") {
           if (font_size == 12) {
               tft.drawRGBBitmap(curr_x, curr_y, (uint16_t*)&bmp565_12pt_star_pixels[0], bmp565_12pt_star_width, bmp565_12pt_star_height);
               curr_x += char_width;
           } else if (font_size == 18) {
               // not impremented
           }
       }
    }
}
void screen_redraw_item_playgame() {
    // high : 52, 18, right align, 3digit
    String high = "   " + String(playgame_highsrore[curr_menu_id]);
    draw_string(52, 16, 12, high.substring(high.length()-3));
    // point : 123, 18, right align, 3digit
    String point = "   " + String(playgame_point);
    draw_string(123, 16, 12, point.substring(point.length()-3));
    // exercise : 3, 38 left align
    String exercise = String(playgame_value1);
    exercise += (curr_menu_id==menu_add1||curr_menu_id==menu_add2)? "+" : "";
    exercise += (curr_menu_id==menu_sub1||curr_menu_id==menu_sub2)? "-" : "";
    exercise += (curr_menu_id==menu_mul1||curr_menu_id==menu_mul2)? "*" : "";
    exercise += (curr_menu_id==menu_div1||curr_menu_id==menu_div2)? "/" : "";
    exercise += String(playgame_value2) + "=?";
    draw_string(3, 38, 18, exercise);
    // input : 3, 82
    String input = "        " + String(playgame_input);
    draw_string(3, 82, 18, input.substring(input.length()-8));
}

void screen_redraw_item_result(bool hightscore_up) {
    // high : 53, 12, right align, 3digit
    String high = "   " + String(playgame_highsrore[curr_menu_id]);
    high += hightscore_up ? "S" : "";
    draw_string(52, 12, 12, high.substring(high.length()-3));
    // point 94, 37, right align, 3digit
    String point = "   " + String(playgame_point);
    draw_string(94, 37, 18, point.substring(point.length()-3));
}

void screen_keyevent_playgame(uint8_t key_no, uint32_t curr_millis) {
    if (key_no == 10) {
        // back
        playgame_input /= 10;
    } else if (key_no == 11){
        // ok
        if (playgame_input == playgame_answer) {
            // correct
            playgame_point++;
            playgame_input = 0;
            gen_exercise(&playgame_value1, &playgame_value2, &playgame_answer);
            tone_melody.play_tone_melody_async(&correct_melody);
        } else {
            // incorrect
            playgame_input = 0;
            tone_melody.play_tone_melody_async(&incorrect_melody);
        }
    } else if (0<=key_no && key_no<=9) {
        // input
        if (playgame_input < 1000000) {
            playgame_input = playgame_input*10 + key_no;
        }
    }
    screen_redraw_item_playgame();
}

void screen_keyevent_result(uint8_t key_no, uint32_t curr_millis) {
    tone_melody.play_tone_melody_async(&silent_melody);
    curr_screen_id = screen_menu;
    tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_menu_pixels[0], bmp565_menu_width, bmp565_menu_height);
    uint16_t y = ((137-37)/4 * (curr_menu_id/2)) + 37;
    uint16_t x = ((141-103) * (curr_menu_id&1)) + 103;
    tft.fillCircle(x, y, 3, RGB565_TO_BGR565(ST77XX_RED));
}

void screen_timerevent_countdown(uint32_t curr_millis) {
    if (countdown_next_millis==0 || curr_millis < countdown_next_millis) {
        return;
    }
    uint16_t x = (SCREEN_WIDTH - font12_width) /2;
    uint16_t y = (SCREEN_HEIGHT - font12_height) /2;
    tft.drawRGBBitmap(x,y, (uint16_t*)font18_pixels[countdown_next_count], font18_width, font18_height);
    countdown_next_millis += 1000;
    if (countdown_next_count-- == 0) {
        countdown_next_millis = 0;
        curr_screen_id = screen_playgame;
        playgame_point = 0;
        playgame_value1 = 0;
        playgame_value2 = 0;
        playgame_answer = 0;
        playgame_next_millis = curr_millis + 100;
        playgame_start_millis = curr_millis;
        tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_playgame_pixels[0], bmp565_playgame_width, bmp565_playgame_height);
        tft.fillRect(playgeme_bar_padding, playgeme_bar_padding, SCREEN_WIDTH-playgeme_bar_padding*2, playgame_bar_height, RGB565_TO_BGR565(ST77XX_BLUE));
        gen_exercise(&playgame_value1, &playgame_value2, &playgame_answer);
        screen_redraw_item_playgame();
    }
}

void screen_timerevent_playgame(uint32_t curr_millis) {
    if (playgame_next_millis==0 || curr_millis < playgame_next_millis) {
        return;
    }
    uint32_t past_millis = curr_millis - playgame_next_millis - 100;
    if (curr_millis < playgame_start_millis+playgeme_time_limit) {
        uint32_t remain_millis = playgame_start_millis+playgeme_time_limit - curr_millis;
        uint32_t bar_length = SCREEN_WIDTH-playgeme_bar_padding*2;
        uint32_t bar_remain = remain_millis * bar_length / playgeme_time_limit;
        uint32_t bar_padding = bar_length - bar_remain;
        tft.fillRect(playgeme_bar_padding, playgeme_bar_padding, bar_remain, playgame_bar_height, RGB565_TO_BGR565(ST77XX_BLUE));
        tft.fillRect(playgeme_bar_padding + bar_remain, playgeme_bar_padding, bar_padding, playgame_bar_height, RGB_TO_BGR565(128,128,128));
        playgame_next_millis += 100;
    } else {
        bool hightscore_up = false;
        curr_screen_id = screen_result;
        tft.drawRGBBitmap(0,0, (uint16_t*)&bmp565_result_pixels[0], bmp565_result_width, bmp565_result_height);
        if (playgame_highsrore[curr_menu_id] <  playgame_point) {
            tone_melody.play_tone_melody_async(&fanfare_melody);
            playgame_highsrore[curr_menu_id] = playgame_point;
            write_highscore();
            hightscore_up = true;
        } else {
            tone_melody.play_tone_melody_async(&oneup_melody);
        }
        screen_redraw_item_result(hightscore_up);
        delay(1500);
    }
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
