//
// Created by yanxi on 2019/12/14.
//

#ifndef OPENSLRECORDER_RECORDBUFFER_H
#define OPENSLRECORDER_RECORDBUFFER_H


class RecordBuffer {

public:
    short **buffer;
    int index = -1;

    RecordBuffer(int bufferSize);
    ~RecordBuffer();

    short *getRecordBuffer();

    short *getNowBuffer();
};


#endif //OPENSLRECORDER_RECORDBUFFER_H
