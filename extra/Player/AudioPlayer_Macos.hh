
#pragma once

#include <mutex>
#include <AudioToolbox/AudioToolbox.h>
#include "AudioPlayer_Interface.hh"
#include "VolsungHeader.hh"

class Ringbuffer {
    const size_t max_size = 4096;
    size_t head = 0;
    size_t tail = 0;

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
    AudioStreamBasicDescription stream_desc = { 0 };
    AudioQueueBufferRef buffers[4];

    AudioQueueOutputCallback callback = [] (void* user, AudioQueueRef queue, AudioQueueBufferRef buffer) {
        auto* player = (AudioPlayer*) user;
        static int phase = 0;
        float* out_buffer = (float*) buffer->mAudioData;
        for (size_t n = 0; n < AudioPlayer::blocksize / sizeof(float); n++) {
            out_buffer[n] = player->ringbuffer.read_sample();
        }
        AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
        std::cout << "Callback!" << std::endl;
    };

public:
    Ringbuffer ringbuffer;

    void initialize(unsigned channels) override {
        int errors = 0;
        stream_desc.mSampleRate = Volsung::sample_rate;
        stream_desc.mBitsPerChannel = 8 * sizeof (float);
        stream_desc.mBytesPerFrame = sizeof (float);
        stream_desc.mBytesPerPacket = sizeof (float);
        stream_desc.mChannelsPerFrame = 1;
        stream_desc.mFramesPerPacket = 1;
        stream_desc.mFormatID = kAudioFormatLinearPCM;
        stream_desc.mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
        errors += AudioQueueNewOutput(&stream_desc, callback, this, NULL, NULL, 0, &queue);

        for (auto& buffer: buffers) {
            errors += AudioQueueAllocateBuffer(queue, blocksize, &buffer);
            buffer->mAudioDataByteSize = blocksize;
            callback(this, queue, buffer);
        }

        //AudioQueueEnqueueBuffer(queue, buffers[0], 0, nullptr);
        AudioQueueStart(queue, NULL);
        std::cout << "Error code: " << errors << std::endl;
    }

    void play(float* data) override {
        while (ringbuffer.capacity() < ringbuffer.size() + blocksize);
        for (size_t n = 0; n < blocksize; n++)
            ringbuffer.submit_sample(data[n]);
    }

    void clean_up() override {
        AudioQueueStop(queue, false);
        AudioQueueDispose(queue, false);
    }
};
