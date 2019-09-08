//
// Created by yanxi on 2019/8/25.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {

    while (!isExit) {
        XData data = Read();
        LOGI("IDemux Read %d:",data.size);
        if (data.size > 0){
            notify(data);
        }
//        if(data.size == 0){
//            break;
//        }
    }
}
