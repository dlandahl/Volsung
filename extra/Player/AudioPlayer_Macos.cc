
#include "AudioPlayer_Macos.hh"

Ringbuffer::Ringbuffer() {
    data = new float[max_size];
    for (size_t n = 0; n < max_size; n++) data[n] = 0.f;
    std::cout << "constructed!" << std::endl;
}

Ringbuffer::~Ringbuffer() {
    delete[] data;
}

__attribute__((noinline)) void Ringbuffer::submit_sample(float sample) {
    const std::lock_guard<std::mutex> lock(mutex);
    data[head] = sample;
    if (full) {
        tail = (tail + 1) % max_size;
        std::cout << "Buffer overflow detected!" << std::endl;
    }
    head = (head + 1) % max_size;
    full = head == tail;
    std::cout << "Wrote sample! Delay is now: " << size() << std::endl;
}

__attribute__((noinline)) float Ringbuffer::read_sample() {
    const std::lock_guard<std::mutex> lock(mutex);
    if (empty()) {
        // std::cout << "Buffer underflow detected!" << std::endl;
        return 0.f;
    }
    float value = data[tail];
    full = false;
    tail = (tail + 1) % max_size;
    std::cout << "Read sample! Delay is now: " << size() << std::endl;
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
