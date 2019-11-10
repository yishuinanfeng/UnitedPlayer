//
// Created by yanxi on 2019/8/25.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {

    while (!isExit) {
        if (IsPause()){
            Sleep(2);
            continue;
        }
        XData data = Read();
    //    LOGI("IDemux Read %d:",data.size);
        if (data.size > 0){
            notify(data);
        } else{
            Sleep(10);
        }
//        if(data.size == 0){
//            break;
//        }
    }
}
