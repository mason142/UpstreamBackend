#ifndef STREAMVIDEOBUFFER_H
#define STREAMVIDEOBUFFER_H
#include <iostream>
#include <sstream>
#include <vector>
#include <assert.h>

class StreamVideoBuffer {
    public:
        char *sps;
        char *pps;
        char *preIFrame;
        char *iFrame;
        char **pFrame;
        int hasWriter;
        int numPFrames;
        int sizeSPS;
        int sizePPS;
        int sizeiFrame;
        int sizepreIFrame;
        int *sizepFrame;

        StreamVideoBuffer();
        int getSize();
        bool addSPS(std::vector<char> data);
        bool addPPS(std::vector<char> data);
        bool addpreIFrame(std::vector<char> data);
        bool addIFrame(std::vector<char> data);
        bool addpFrame(std::vector<char> data);
        bool clean();
};
#endif