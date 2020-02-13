
#pragma once

struct AudioPlayer_Interface
{
    static const size_t blocksize = 512;
    virtual void initialize(int) = 0;
    virtual void play(float*) = 0;
    virtual void clean_up() = 0;
};

