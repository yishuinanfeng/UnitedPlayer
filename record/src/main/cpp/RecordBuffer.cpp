//
// Created by yanxi on 2019/12/14.
//

#include "RecordBuffer.h"

RecordBuffer::RecordBuffer(int bufferSize) {
    //两个Buffer，一个用来显示声音，一个用来处理数据
    buffer = new short *[2];
    for (int i = 0; i < 2; ++i) {
        buffer[i] = new short[bufferSize];
    }
}

RecordBuffer::~RecordBuffer() {
    for (int i = 0; i < 2; ++i) {
        delete buffer[i];
    }
}

short *RecordBuffer::getRecordBuffer() {
    index++;
    if (index > 1) {
        index = 1;
    }
    return buffer[index];
}

short *RecordBuffer::getNowBuffer() {
    return buffer[index];
}
