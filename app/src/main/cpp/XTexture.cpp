//
// Created by yanxi on 2019/9/15.
//

#include "XTexture.h"
#include "XEGL.h"
#include "XLog.h"
#include "XShader.h"

class CXTexture : public XTexture {
public:
    XShader xShader;
    XTextureType type;
    std::mutex mutex;

    virtual void Drop() {
      //  mutex.lock();
        const std::lock_guard<std::mutex> lock(mutex);
        XEGL::Get()->Close();
        xShader.Close();
     //   mutex.unlock();
        //todo 相当于调用析构函数？
        delete this;
    }

    virtual bool Init(void *win, XTextureType textureType) {
    //    mutex.lock();
        const std::lock_guard<std::mutex> lock(mutex);
        XEGL::Get()->Close();
        xShader.Close();
        this->type = textureType;
        if (!win) {
            LOGE("CXTexture win is NULL！");
        }
        if (!XEGL::Get()->Init(win)) {
       //     mutex.unlock();
            return false;
        }

        LOGE("Init xShader");
        bool isInit = xShader.Init(static_cast<XShaderType>(type));
    //    mutex.unlock();
        return isInit;
    }

    virtual void Draw(unsigned char *data[], int width, int height) {
     //   mutex.lock();
        const std::lock_guard<std::mutex> lock(mutex);
        xShader.GetTexture(0, width, height, data[0]);//Y
        LOGDT("GetTexture data y:%d:", *data[0]);
        if (type == XTEXTURE_YUV420P) {
            xShader.GetTexture(1, width / 2, height / 2, data[1]);//U
            LOGDT("GetTexture data u:%d:", *data[1]);
            xShader.GetTexture(2, width / 2, height / 2, data[2]);//V
            LOGDT("GetTexture data v:%d:", *data[2]);
        } else {
            //NV12和NV21解码出来为Y和U两个数组
            xShader.GetTexture(1, width / 2, height / 2, data[1], true);//UV
        }

        xShader.Draw();
        XEGL::Get()->Draw();
   //     mutex.unlock();
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}


