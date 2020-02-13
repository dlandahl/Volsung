
#pragma once

struct AudioPlayer_Interface
{
    static const size_t blocksize = 1024;
    virtual void initialize(unsigned) = 0;
    virtual void play(float*) = 0;
    virtual void clean_up() = 0;
};

