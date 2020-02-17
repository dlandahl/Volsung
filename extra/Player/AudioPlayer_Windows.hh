
#pragma once

#include "AudioPlayer_Interface.hh"

#include "Audioclient.h"
#include "Mmdeviceapi.h"

class AudioPlayer : public AudioPlayer_Interface
{
    IAudioClient* client = nullptr;
    IMMDevice* device = nullptr;
    IMMDeviceEnumerator* enumerator = nullptr;
    uint32_t buffersize_frames = 0;
public:
    void initialize(Volsung::uint channels) override {
        Volsung::sample_rate = 48000;
        channels = 2;

        CoInitialize(nullptr);
        std::cout << CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**) &enumerator) << std::endl;
        std::cout << enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device) << std::endl;
        std::cout << device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, (void**) &client) << std::endl;

        WAVEFORMATEX format;
        memset(&format, 0, sizeof(WAVEFORMATEX));
        format.wFormatTag      = WAVE_FORMAT_PCM;
        format.nChannels       = (WORD) channels;
        format.nSamplesPerSec  = (DWORD) Volsung::sample_rate;
        format.nAvgBytesPerSec = (DWORD) Volsung::sample_rate * sizeof(int16_t) * channels;
        format.nBlockAlign     = (WORD) (sizeof(int16_t) * channels);
        format.wBitsPerSample  = 16;

        const size_t buffersize_samples = blocksize * channels;
        const size_t buffersize_nanoseconds = buffersize_samples / Volsung::sample_rate * 1000000000.;
        std::cout << client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, (REFERENCE_TIME) (buffersize_nanoseconds / 100.f), 0, &format, 0) << std::endl;

        client->GetBufferSize(&buffersize_frames);
        std::cout << buffersize_frames << std::endl;
    }

    void play(float* data) override {
    }

    void clean_up() override {
        if (client) client->Release();
    }
};
