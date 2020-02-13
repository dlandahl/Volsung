
#pragma once

#include "AudioPlayer_Interface.hh"
#include "alsa/asoundlib.h"

class AudioPlayer : public AudioPlayer_Interface
{
    snd_pcm_t *stream;

public:
    void initialize(int channels) override {
        snd_pcm_open(&stream, "default", SND_PCM_STREAM_PLAYBACK, 0);

        snd_pcm_set_params(stream, SND_PCM_FORMAT_FLOAT_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                           channels, Volsung::sample_rate, 1, 10000);
    }

    void play(float* data) override {
        snd_pcm_sframes_t delay = 0;
        do {
            snd_pcm_delay(stream, &delay);
        } while (delay > (long) blocksize * 2);
        snd_pcm_writei(stream, data, blocksize);
    }

    void clean_up() override {
        snd_pcm_drain(stream);
        snd_pcm_close(stream);
    }
};
