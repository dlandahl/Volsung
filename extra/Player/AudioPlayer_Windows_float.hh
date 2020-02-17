
#pragma once

#include "AudioPlayer_Interface.hh"

#include "dsound.h"
#include "mmreg.h"
#include "cguid.h"

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
        buffersize_bytes = blocksize * channels * sizeof(float) * 4;
        DirectSoundCreate8(NULL, &device, NULL);

		device->SetCooperativeLevel(get_console_window(), DSSCL_NORMAL);

		WAVEFORMATIEEEFLOATEX format;
		memset(&format, 0, sizeof(WAVEFORMATEX));
        format.Format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        format.Format.nChannels = (WORD)channels;
        format.Format.nSamplesPerSec = (DWORD) Volsung::sample_rate;
		format.Format.nAvgBytesPerSec = (DWORD) Volsung::sample_rate * sizeof(float) * channels;
		format.Format.nBlockAlign = (WORD) (sizeof(float) * channels);
		format.Format.wBitsPerSample = 32;
        format.Format.cbSize = 22;
		format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        format.Samples.wValidBitsPerSample = 32;
        format.dwChannelMask = 0;

		DSBUFFERDESC desc;
		memset(&desc, 0, sizeof(DSBUFFERDESC));
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = 0;
		desc.dwBufferBytes = sizeof(float) * blocksize * 4 * channels;
		desc.dwReserved = 0;
		desc.lpwfxFormat = &format.Format;

		const char guid_null[16] = { 0 };
		desc.guid3DAlgorithm = *(GUID*) (guid_null);

		device->CreateSoundBuffer(&desc, &buffer, NULL);

		buffer->SetCurrentPosition(0);
		buffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	void play(float* data) override {
		DWORD play_position;
        size_t delay = 0;
        do {
            buffer->GetCurrentPosition(&play_position, nullptr);
            delay = write_pointer > play_position ?
                write_pointer - play_position : (buffersize_bytes - play_position) + write_pointer;
        } while (delay < blocksize * channels * sizeof(float));

        void* chunk_a;
        void* chunk_b;
        DWORD chunk_a_size;
        DWORD chunk_b_size;

        buffer->Lock(write_pointer, blocksize * channels * sizeof (float), &chunk_a, &chunk_a_size, &chunk_b, &chunk_b_size, 0);
        for (size_t n = 0; n < chunk_a_size / sizeof (float); n++) {
            for (size_t c = 0; c < channels; c++) {
                ((float*) chunk_a)[n * channels + c] = data[n];
            }
        }

        for (size_t n = 0; n < chunk_b_size / sizeof(float); n++) {
            for (size_t c = 0; c < channels; c++) {
                ((float*)chunk_b)[n * channels + c] = data[n + chunk_a_size / sizeof (float)];
            }
        }

        buffer->Unlock(chunk_a, chunk_a_size, chunk_b, chunk_b_size);
        write_pointer += sizeof (float) * blocksize * channels;
        if (write_pointer >= buffersize_bytes) write_pointer -= buffersize_bytes;
    }

    void clean_up() override {
    }
};
