
#pragma once

#include "AudioPlayer_Interface.hh"

#include "dsound.h"
#include "mmreg.h"
#include "cguid.h"
#include "synchapi.h"

HWND get_console_window()
{
    unsigned const title_len = 128u;
    TCHAR console_title[title_len];

    GetConsoleTitle(console_title, title_len);
    HWND window_handle = FindWindow(NULL, console_title);

    return window_handle;
}

class AudioPlayer : public AudioPlayer_Interface
{
    LPDIRECTSOUND8 device = nullptr;
	LPDIRECTSOUNDBUFFER buffer = nullptr;
	size_t write_pointer = 0;
    size_t buffersize_bytes;

    size_t channels;

public:
    void initialize(Volsung::uint _channels) override {
        channels = _channels;
        buffersize_bytes = blocksize * channels * sizeof(int16_t) * 8;
        DirectSoundCreate8(NULL, &device, NULL);

        device->SetCooperativeLevel(get_console_window(), DSSCL_NORMAL);

		WAVEFORMATEX format;
		memset(&format, 0, sizeof(WAVEFORMATEX));
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = (WORD) channels;
		format.nSamplesPerSec = (DWORD) Volsung::sample_rate;
		format.nAvgBytesPerSec = (DWORD) Volsung::sample_rate * sizeof(int16_t) * channels;
		format.nBlockAlign = (WORD) (sizeof(int16_t) * channels);
		format.wBitsPerSample = 16;

		DSBUFFERDESC desc;
		memset(&desc, 0, sizeof(DSBUFFERDESC));
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = 0;
		desc.dwBufferBytes = buffersize_bytes;
		desc.dwReserved = 0;
		desc.lpwfxFormat = &format;

		const char guid_null[16] = { 0 };
		desc.guid3DAlgorithm = *(GUID*)(guid_null);

		device->CreateSoundBuffer(&desc, &buffer, NULL);

		buffer->SetCurrentPosition(0);
		buffer->Play(0, 0, DSBPLAY_LOOPING);
	}

    void play(float* data) override {
        DWORD play_position;
        size_t delay;
        int16_t int_data[blocksize];
        for (size_t n = 0; n < blocksize; n++) {
            int_data[n] = data[n] * 10000;
        }

        do {
            buffer->GetCurrentPosition(&play_position, nullptr);
            delay = write_pointer > play_position ?
                write_pointer - play_position : (buffersize_bytes - play_position) + write_pointer;
        } while (delay > (long) blocksize * sizeof(int16_t) * 4);

        void* chunk_a;
        void* chunk_b;
        DWORD chunk_a_size;
        DWORD chunk_b_size;

        buffer->Lock(write_pointer, blocksize * sizeof(int16_t), &chunk_a, &chunk_a_size, &chunk_b, &chunk_b_size, 0);
        for (size_t n = 0; n < chunk_a_size / sizeof(int16_t); n++) {
            ((int16_t*)chunk_a)[n] = int_data[n];
        }

        for (size_t n = 0; n < chunk_b_size / sizeof(int16_t); n++) {
            ((int16_t*)chunk_b)[n] = int_data[n + chunk_a_size / sizeof(int16_t)];
        }

        buffer->Unlock(chunk_a, chunk_a_size, chunk_b, chunk_b_size);
        write_pointer += sizeof(int16_t) * blocksize;
        if (write_pointer >= buffersize_bytes) write_pointer -= buffersize_bytes;
    }

    void clean_up() override {
    }
};
