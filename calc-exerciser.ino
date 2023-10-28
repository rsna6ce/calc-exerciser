#include "esp_system.h"
#include "tone_melody.h"

const uint32_t buz_pin = 13;
const uint32_t buz_ch = 0;


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
    {tone_fa, delay2},
};
struct melody_t chaim_melody = TONES_TO_MELODY(chaim_tones);

// correct
tone_t correct_tones[] = {
    {tone_si2, delay0},
    {tone_so2, delay1},
};
struct melody_t correct_melody = TONES_TO_MELODY(correct_tones);

// incorect
tone_t incorrect_tones[] = {
    {tone_do, delay2},
};
struct melody_t incorrect_melody = TONES_TO_MELODY(incorrect_tones);

void setup() {
    Serial.begin(115200);
    Serial.println("calc exerciser!");

    ledcSetup(buz_ch, 12000, 8);
    ledcAttachPin(buz_pin, buz_ch);

    xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 2, NULL, 0);
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
        play_tone_melody(buz_ch, &chaim_melody);
        delay(delay2*2);
        
        play_tone_melody(buz_ch, &correct_melody);
        delay(delay2*2);
        
        play_tone_melody(buz_ch, &incorrect_melody);
        delay(delay2*2);
    }
}
