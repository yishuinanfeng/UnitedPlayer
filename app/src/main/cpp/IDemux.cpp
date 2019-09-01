//
// Created by yanxi on 2019/8/25.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {

    for (;;) {
        XData data = Read();
        LOGI("IDemux Read %d:",data.size);
        if(data.size == 0){
            break;
        }
    }
}
