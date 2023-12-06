#include <iostream>
#include <sstream>
#include <vector>
#include <assert.h>

class SlowStreamCircularBuffer {
    public:
        int currentWriterFrame;
        int maxFrames;
        char **frameData;
        int *frameSize;
        SlowStreamCircularBuffer(int maxFrames);
        bool allocateFrames();
        bool appendFrame(std::vector<char> data);
};