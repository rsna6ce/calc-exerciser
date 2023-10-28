#pragma once

#define TONES_TO_MELODY(x) {x,sizeof(x)/sizeof(x[0])}

static const uint32_t tone_no  = 0;   // no-sound
static const uint32_t tone_do  = 262; // ド
static const uint32_t tone_re  = 294; // レ
static const uint32_t tone_mi  = 330; // ミ
static const uint32_t tone_fa  = 349; // ファ
static const uint32_t tone_so  = 392; // ソ
static const uint32_t tone_ra  = 440; // ラ
static const uint32_t tone_si  = 494; // シ
static const uint32_t tone_do2 = 523; // ド
static const uint32_t tone_re2 = 587; // レ
static const uint32_t tone_mi2 = 659; // ミ
static const uint32_t tone_fa2 = 698; // ファ
static const uint32_t tone_so2 = 784; // ソ
static const uint32_t tone_ra2 = 880; // ラ
static const uint32_t tone_si2 = 988; // シ
static const uint32_t tone_do3 = 1049;// ド
static const uint32_t delay1 = 300;
static const uint32_t delay2 = 480;
static const uint32_t delay0 = delay1/2;

struct tone_t{
    uint32_t freq;
    uint32_t lengh_ms;
} ;
struct melody_t{
    struct tone_t* tone;
    uint32_t count;
};

static void play_tone_melody(uint32_t ch, struct melody_t* melody) {
    for (uint32_t i=0; i<melody->count; i++) {
        uint32_t freq = melody->tone[i].freq;
        uint32_t len = melody->tone[i].lengh_ms;
        ledcWriteTone(ch, freq);
        delay(len);
    }
    ledcWriteTone(ch, tone_no);
}
