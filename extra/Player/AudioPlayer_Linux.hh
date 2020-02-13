
#pragma once

#include "AudioPlayer_Interface.hh"
#include "alsa/asoundlib.h"

class AudioPlayer : public AudioPlayer_Interface
{
    snd_pcm_t *stream;

public:
    void initialize(unsigned channels) override {
        snd_pcm_open(&stream, "default", SND_PCM_STREAM_PLAYBACK, 0);

        snd_pcm_hw_params_t *params;
        snd_pcm_hw_params_alloca ( &params );

        snd_pcm_hw_params_any(stream, params);
        snd_pcm_hw_params_set_format(stream, params, SND_PCM_FORMAT_FLOAT_LE);
        snd_pcm_hw_params_set_rate(stream, params, Volsung::sample_rate, 0);
        snd_pcm_hw_params_set_channels_minmax(stream, params, &channels, &channels);

        snd_pcm_hw_params(stream, params);
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

