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

    virtual bool Init(void *win, XTextureType textureType) {
        this->type = textureType;
        if (!win) {
            LOGE("CXTexture win is NULL！");
        }
        if (!XEGL::Get()->Init(win)) {
            return false;
        }

        LOGE("Init xShader");
        return xShader.Init(static_cast<XShaderType>(type));
    }

    virtual void Draw(unsigned char *data[], int width, int height) {
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
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}
