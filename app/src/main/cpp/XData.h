//
// Created by yanxi on 2019/8/25.
//

#ifndef MANCHESTERUNITEDPLAYER_XDATA_H
#define MANCHESTERUNITEDPLAYER_XDATA_H
/**
 * 解复用解码不同模块间传递用得的封装数据类
 */
struct XData {
    unsigned char *data = 0;
    unsigned char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    int width;
    int height;
    void Drop();
};


#endif //MANCHESTERUNITEDPLAYER_XDATA_H
