
#include "AudioPlayer_Macos.hh"

Ringbuffer::Ringbuffer()
{
    data = new float[max_size];
    for (size_t n = 0; n < max_size; n++) data[n] = 0.f;
}

Ringbuffer::~Ringbuffer()
{
    delete[] data;
}

void Ringbuffer::submit_sample(float sample)
{
    const std::lock_guard<std::mutex> lock(mutex);
    data[head] = sample;
    if (full) {
        tail = (tail + 1) % max_size;
    }
    head = (head + 1) % max_size;
    full = head == tail;
}

float Ringbuffer::read_sample()
{
    const std::lock_guard<std::mutex> lock(mutex);
    if (empty()) {
        underflow_flag = true;
        return 0.f;
    }
    float value = data[tail];
    full = false;
    tail = (tail + 1) % max_size;
    return value;
}

bool Ringbuffer::empty() const
{
    return (!full && (head == tail));
}

size_t Ringbuffer::capacity() const
{
    return max_size;
}

size_t Ringbuffer::size() const
{
    if (full) return max_size;
    if (head >= tail) return head - tail;
    return max_size + head - tail;
}
