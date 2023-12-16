#ifndef STREAMDATABASE_H
#define STREAMDATABASE_H

#include "./StreamVideoBuffer.h"

class StreamDatabase {
    public:
        StreamDatabase(int maxStreams, int maxBuff);
        StreamDatabase();

        ~StreamDatabase();

        int createNewLivestreamingSession(std::vector<char> title);

        int deleteLivestreamingSession(int id);

        int writeEncodedData(int id, int type, std::vector<char> data);

        int readEncodedData(int id);

        bool isIDactive(int id);


    private:
        std::map<int, std::vector<StreamVideoBuffer*>> _liveStreams;
        std::map<int, int> _stvbInds;
        int _maxStreams;
        int _maxBuff;
        int _id;
};
#endif // STREAMDATABASE_H