//
// Created by yanxi on 2019/9/15.
//

#include <GLES2/gl2.h>
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
        const std::lock_guard<std::mutex> lock(mutex);
        XEGL::Get()->Close();
        xShader.Close();

        delete this;
    }

    virtual bool Init(void *win, XTextureType textureType, int filterType) {

        const std::lock_guard<std::mutex> lock(mutex);
        XEGL::Get()->Close();
        xShader.Close();
        this->type = textureType;
        if (!win) {
            LOGE("CXTexture win is NULL！");
        }
        if (!XEGL::Get()->Init(win)) {
            return false;
        }

        LOGE("Init xShader");
        bool isInit = xShader.Init(static_cast<XShaderType>(type), filterType);
        //    mutex.unlock();
        return isInit;
    }

    virtual void Draw(unsigned char *data[], int videoWidth, int videoHeight, int pts, int screenWidth,
                      int screenHeight) {
        //   mutex.lock();
        const std::lock_guard<std::mutex> lock(mutex);
        adjustVideoDimension(videoWidth, videoHeight, screenWidth, screenHeight);

        xShader.GetTexture(0, videoWidth, videoHeight, data[0]);//Y
        LOGDT("GetTexture data y:%d:", *data[0]);
        if (type == XTEXTURE_YUV420P) {
            xShader.GetTexture(1, videoWidth / 2, videoHeight / 2, data[1]);//U
            LOGDT("GetTexture data u:%d:", *data[1]);
            xShader.GetTexture(2, videoWidth / 2, videoHeight / 2, data[2]);//V
            LOGDT("GetTexture data v:%d:", *data[2]);
        } else {
            //NV12和NV21解码出来为Y和UV两个数组
            xShader.GetTexture(1, videoWidth / 2, videoHeight / 2, data[1], true);//UV
        }

        xShader.Draw(pts);
        XEGL::Get()->Draw();
    }

    /**
     * 视频尺寸适配屏幕
     * 适配策略（类似ImageView的fitCenter效果）：
     * 1.屏幕宽高比大于视频宽高比，视频填充高度
     * 2.屏幕宽高比小于视频宽高比，视频填充宽度
     *
     * @param videoWidth
     * @param videoHeight
     * @param screenWidth
     * @param screenHeight
     */
    virtual void adjustVideoDimension(int videoWidth, int videoHeight, int screenWidth, int screenHeight) {
        //分别算出屏幕和视频宽高比
        float screenDimensionRatio = screenWidth * 1.0F / screenHeight;
        float videoDimensionRatio = videoWidth * 1.0F / videoHeight;

        //视口的视频左下角点
        int viewPortX;
        int viewPortY;

        GLsizei actualVideoHeight;
        GLsizei actualVideoWidth;

        if (screenDimensionRatio > videoDimensionRatio) {
            //屏幕宽高比大于视频宽高比，视频填充高度
            float ratio = screenHeight * 1.0F / videoHeight;
            actualVideoHeight = screenHeight;
            actualVideoWidth = static_cast<GLsizei>(videoWidth * ratio);

            int halfScreenWidth = screenWidth / 2;
            viewPortX = halfScreenWidth - actualVideoWidth / 2;
            viewPortY = 0;
        } else {
            //屏幕宽高比小于视频宽高比，视频填充宽度
            float ratio = screenWidth * 1.0F / videoWidth;
            actualVideoHeight = static_cast<GLsizei>(videoHeight * ratio);
            actualVideoWidth = screenWidth;

            int halfScreenHeight = screenHeight / 2;
            viewPortY = halfScreenHeight - actualVideoHeight / 2;
            viewPortX = 0;
        }
        //todo 是否有必要每帧都需要设置视口？
        glViewport(viewPortX, viewPortY, actualVideoWidth, actualVideoHeight);
        LOG_VIDEO_DIMENSION("Draw：width:%d,height:%d", actualVideoWidth, actualVideoHeight);
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}


