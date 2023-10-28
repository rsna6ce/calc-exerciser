#include "tone_melody.h"

const uint32_t buz_pin = 13;
const uint32_t buz_ch = 0;


void setup() {
    Serial.begin(115200);
    Serial.println("calc exerciser!");

    ledcSetup(buz_ch, 12000, 8);
    ledcAttachPin(buz_pin, buz_ch);
}

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



void loop(void) {
    play_tone_melody(buz_ch, &chaim_melody);
    delay(delay2*2);
    
    play_tone_melody(buz_ch, &correct_melody);
    delay(delay2*2);
    
    play_tone_melody(buz_ch, &incorrect_melody);
    delay(delay2*2);
}
