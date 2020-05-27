
#pragma once

#include <mutex>
#include <AudioToolbox/AudioToolbox.h>
#include "AudioPlayer_Interface.hh"
#include "Volsung.hh"

class Ringbuffer {
    const size_t max_size = 4096;
    size_t head = 0;
    size_t tail = 0;

    bool underflow_flag = false;
    bool full = false;
    std::mutex mutex;

    float* data = nullptr;

public:
    Ringbuffer();
    ~Ringbuffer();
    
    void submit_sample(float);
    float read_sample();
    bool empty() const;
    size_t capacity() const;
    size_t size() const;
};

class AudioPlayer : public AudioPlayer_Interface
{
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[4];
    Ringbuffer ringbuffer;

    AudioQueueOutputCallback callback = [] (void* user, AudioQueueRef queue, AudioQueueBufferRef buffer)
    {
        auto* player = (AudioPlayer*) user;
        float* out_buffer = (float*) buffer->mAudioData;
        for (size_t n = 0; n < AudioPlayer::blocksize / sizeof(float); n++) {
            out_buffer[n] = player->ringbuffer.read_sample();
        }
        AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
    };

public:
    void initialize(unsigned channels) override
    {
        AudioStreamBasicDescription format = { 0 };

        format.mSampleRate = Volsung::sample_rate;
        format.mBitsPerChannel = 8 * sizeof (float) * channels;
        format.mChannelsPerFrame = channels;
        format.mBytesPerFrame = sizeof (float) * channels;
        format.mBytesPerPacket = format.mBytesPerFrame;
        format.mFramesPerPacket = 1;
        format.mFormatID = kAudioFormatLinearPCM;
        format.mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;

        AudioQueueNewOutput(&format, callback, this, NULL, NULL, 0, &queue);

        for (auto& buffer: buffers) {
            AudioQueueAllocateBuffer(queue, blocksize, &buffer);
            buffer->mAudioDataByteSize = blocksize;
            callback(this, queue, buffer);
        }

        AudioQueueStart(queue, NULL);
    }

    void play(float* data) override
    {
        while (ringbuffer.capacity() < ringbuffer.size() + blocksize);
        for (size_t n = 0; n < blocksize; n++)
            ringbuffer.submit_sample(data[n]);
    }

    void clean_up() override
    {
        AudioQueueStop(queue, false);
        AudioQueueDispose(queue, false);
    }
};

