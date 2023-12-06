#include "include/SlowStreamCircularBuffer.h"
#include <assert.h>

SlowStreamCircularBuffer::SlowStreamCircularBuffer(int mF) {
    maxFrames = mF;
        currentWriterFrame = 0;
}

bool SlowStreamCircularBuffer::allocateFrames() {
    frameData = (char **)malloc(60 * sizeof(char*));
    assert(frameData);
    for (int i = 0; i < maxFrames; i++) {
        frameData[i] = (char *)malloc(1533600 * sizeof(char *));
        assert(frameData[i]);
    }
    frameSize = (int*)malloc(maxFrames * sizeof(int));
    assert(frameSize);
    return true;
}

bool SlowStreamCircularBuffer::appendFrame(std::vector<char> data) {
    int len = data.size();
    frameSize[currentWriterFrame] = len;
    printf("Length = %d\n", len);
    assert(len < 1533600);
    for (int i = 0; i < len; i++) {
        frameData[currentWriterFrame][i] = data[i];
    }
    currentWriterFrame++;
    currentWriterFrame = currentWriterFrame % maxFrames;
    return true;
}