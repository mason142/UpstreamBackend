#include "../include/streamingServer.h"

StreamDatabase::StreamDatabase(int maxStreams, int maxBuff) : _maxStreams(maxStreams), _maxBuff(maxBuff), _id(0) {
}

StreamDatabase::StreamDatabase() : StreamDatabase(10, 100) {
}

StreamDatabase::~StreamDatabase() {
    // todo
}

int StreamDatabase::createNewLivestreamingSession(std::vector<char> title) {
    if (_liveStreams.size() >= _maxStreams) {
        return -1;
    }

    std::vector<StreamVideoBuffer*> *stream = new std::vector<StreamVideoBuffer*>;
    for (int i = 0; i < _maxBuff; i++) {
        StreamVideoBuffer *s = new StreamVideoBuffer;
        stream->push_back(s);
    }

    this->_stvbInds[++_id] = 0;
    this->_liveStreams[_id] = *stream;
    return _id;
}

int StreamDatabase::deleteLivestreamingSession(int id) {
    auto it = _liveStreams.find(id);
    if (it != _liveStreams.end()) {
        for (StreamVideoBuffer* buffer : it->second) {
            delete buffer;
        }

        delete &it->second;
        _liveStreams.erase(it);

        return 0;  // Success
    }
    return -1;
}

int StreamDatabase::writeEncodedData(int id, int type, std::vector<char> data) {
    std::vector<StreamVideoBuffer*> stream = _liveStreams[id]; //hardcoded...
    StreamVideoBuffer *stvb = stream[_stvbInds[id]];
    if (!stvb) {
        return -1;
    }
    if (type == 7) {
        stvb->hasWriter = 0;
        _stvbInds[id] = ((_stvbInds[id] + 1) % _maxBuff);
        std::cout << "In SDB, Recording stvbInd = " << (_stvbInds[id] % _maxBuff) << "\n";
        stvb = stream[(_stvbInds[id] % _maxBuff)];
        assert(stvb);
        stvb->clean();
        if (!stvb->addSPS(data)) {
            std::cout << "\nERROR AT STAGE" << type << "\n";
            return -1;
        }
    }
    if (type == 8) {
        if (!stvb->addPPS(data)) {
            std::cout << "\nERROR AT STAGE" << type << "\n";
            return -1;   
        }
    }
    if (type == 6) {
        //std::cout << "SAVING MYSTERY\n";
        if (!stvb->addpreIFrame(data)) {
            std::cout << "\nERROR AT STAGE" << type << "\n";
            return -1;
        }
    }
    else if (type == 5) {
        //std::cout << "SAVING IFRAME\n";
        if (!stvb->addIFrame(data)) {
            std::cout << "\nERROR AT STAGE" << type << "\n";
            return -1;
        }
    }
    else if (type == 1) {
        //std::cout << "SAVING PFRAME " << pFramesRecv << "\n";
        if (!stvb->addpFrame(data)) {
            std::cout << "\nERROR AT STAGE" << type << "\n";
            return;
        }
    }
    return 1;  // Placeholder for success
}

int StreamDatabase::readEncodedData(int id) {
    // Implementation for reading encoded data, if needed
    return 0;  // Placeholder for success
}

bool StreamDatabase::isIDactive(int id) {
    return _liveStreams.find(id) != _liveStreams.end();
}