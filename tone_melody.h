#pragma once
#include "esp_system.h"
#include <stdint.h>

#define TONES_TO_MELODY(x) {x,sizeof(x)/sizeof(x[0])}

static const float tone_base_do = 261.626;
static const float tone_base_re = 293.665;
static const float tone_base_mi = 329.628;
static const float tone_base_fa = 349.228;
static const float tone_base_so = 391.995;
static const float tone_base_ra = 440.000;
static const float tone_base_si = 493.883;

static const uint32_t tone_no  = 0;   // no-sound
static const uint32_t tone_do  = (uint32_t)(tone_base_do * 1.0); // ド
static const uint32_t tone_re  = (uint32_t)(tone_base_re * 1.0); // レ
static const uint32_t tone_mi  = (uint32_t)(tone_base_mi * 1.0); // ミ
static const uint32_t tone_fa  = (uint32_t)(tone_base_fa * 1.0); // ファ
static const uint32_t tone_so  = (uint32_t)(tone_base_so * 1.0); // ソ
static const uint32_t tone_ra  = (uint32_t)(tone_base_ra * 1.0); // ラ
static const uint32_t tone_si  = (uint32_t)(tone_base_si * 1.0); // シ
static const uint32_t tone_do2 = (uint32_t)(tone_base_do * 2.0); // ド
static const uint32_t tone_re2 = (uint32_t)(tone_base_re * 2.0); // レ
static const uint32_t tone_mi2 = (uint32_t)(tone_base_mi * 2.0); // ミ
static const uint32_t tone_fa2 = (uint32_t)(tone_base_fa * 2.0); // ファ
static const uint32_t tone_so2 = (uint32_t)(tone_base_so * 2.0); // ソ
static const uint32_t tone_ra2 = (uint32_t)(tone_base_ra * 2.0); // ラ
static const uint32_t tone_si2 = (uint32_t)(tone_base_si * 2.0); // シ
static const uint32_t tone_do3 = (uint32_t)(tone_base_do * 3.0); // ド
static const uint32_t tone_re3 = (uint32_t)(tone_base_re * 3.0); // レ
static const uint32_t tone_mi3 = (uint32_t)(tone_base_mi * 3.0); // ミ
static const uint32_t tone_fa3 = (uint32_t)(tone_base_fa * 3.0); // ファ
static const uint32_t tone_so3 = (uint32_t)(tone_base_so * 3.0); // ソ
static const uint32_t tone_ra3 = (uint32_t)(tone_base_ra * 3.0); // ラ
static const uint32_t tone_si3 = (uint32_t)(tone_base_si * 3.0); // シ
static const uint32_t tone_do4 = (uint32_t)(tone_base_do * 3.0); // ド
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

class ToneMelody{
    public:
        ToneMelody(uint32_t buz_ch, uint32_t buz_pin):
            _is_playing(false),
            _play_job_id(0),
            _buz_ch(buz_ch),
            _buz_pin(buz_pin)
        {};

    private:
        uint32_t _buz_ch;
        uint32_t _buz_pin;
        bool _is_playing;
        uint32_t _play_job_id;
        struct melody_t* _playing_melody;
        TaskHandle_t Task1;

    public: void begin() {
        ledcSetup(_buz_ch, 12000, 8);
        ledcAttachPin(_buz_pin, _buz_ch);

        xTaskCreatePinnedToCore(this->loopTask, "tone_melody_loop", 4096, this, 1, &Task1, 0);
    }
    
    public: void play_tone_melody_sync(struct melody_t* melody) {
        for (uint32_t i=0; i<melody->count; i++) {
            uint32_t freq = melody->tone[i].freq;
            uint32_t len = melody->tone[i].lengh_ms;
            ledcWriteTone(_buz_ch, freq);
            delay(len);
        }
        ledcWriteTone(_buz_ch, tone_no);
    };
    
    public: void play_tone_melody_async(struct melody_t* melody) {
        _playing_melody = melody;
        _play_job_id++;
    }

    private: static void loopTask(void *pvParameters) {
        ToneMelody *l_pThis = (ToneMelody *) pvParameters;
        uint32_t next_millis = 0;
        struct melody_t* melody = NULL;
        uint32_t index = 0;
        uint32_t latest_job_id = 0;
        uint32_t buz_ch = l_pThis->_buz_ch;

        while (true) {
            uint32_t curr_millis = millis();
            if (latest_job_id != l_pThis->_play_job_id) {
                latest_job_id = l_pThis->_play_job_id;
                melody = l_pThis->_playing_melody;
                index = 0;
                ledcWriteTone(buz_ch, melody->tone[index].freq);
                next_millis = curr_millis + melody->tone[index].lengh_ms;
            }
            if (next_millis < curr_millis && latest_job_id!=0) {
                if (index + 1 < melody->count) {
                    index++;
                    ledcWriteTone(buz_ch, melody->tone[index].freq);
                    next_millis = millis() + melody->tone[index].lengh_ms;
                } else if (index + 1 == melody->count) {
                    index++;
                    ledcWriteTone(buz_ch, tone_no);
                }
            }
            delay(1);
        }
    }
};