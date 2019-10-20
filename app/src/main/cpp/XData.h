//
// Created by yanxi on 2019/8/25.
//

#ifndef MANCHESTERUNITEDPLAYER_XDATA_H
#define MANCHESTERUNITEDPLAYER_XDATA_H
/**
 * 区分数据类型
 */
enum XDataType{
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1
};
/**
 * 解复用解码不同模块间传递用得的封装数据类
 */
struct XData {
    int type = 0;
    unsigned char *data = 0;
    unsigned char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    int width;
    int height;
    void Drop();
    bool Alloc(int size, const char *data = 0);
};


#endif //MANCHESTERUNITEDPLAYER_XDATA_H
