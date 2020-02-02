# UnitedPlayer

**一个支持录制和播放音频视频的应用，录制的视频将在播放列表中显示，点击列表项即可播放，并可以选择不同仿抖音滤镜特效。**

博客链接：[介绍一个自己刚出炉的音视频播放录制开源项目](https://blog.csdn.net/sinat_23092639/article/details/104142700)

## 基本技术实现点：

使用Camera+OpenGL ES实时预览画面，使用OpenSl ES录制音频，通过MediaCodec分别对音视频进行编码，并通过MediaMuxer将音视频封装为MP4格式。

使用ffmpeg解复用、解码MP4文件，通过OpenGL ES渲染视频画面，OpenSl ES播放音频，并通过OpenGL片段着色器提供不同的视频仿抖音滤镜效果。

## 功能点：

1. 视频音频播放，支持进度条控制和暂停。（已完成）  
  ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/play.jpg)

2. 视频音频录制。（已完成）

    ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/record.jpg)

3. 视频添加水印。（已经实现，待扩展）

    ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/playProgress.jpg)

4. 视频添加仿抖音滤镜。（已经实现最基础的滤镜：反色、灰度图、闪白、缩放、灵魂出窍、抖动）
   1. 反色：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/oppoColor.gif)
   
   2. 灰度图：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/gray.gif)
   
   3. 闪白：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/splash.gif)
   
   4. 缩放：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/scale.gif)
   
   5. 灵魂出窍：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/soul.gif)
   
   6. 颜色偏移抖动：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/shake.gif)
   

5. 视频添加背景音乐。（待开发）

6. 音频变调。（待开发）

7. 视频断点续录。（待开发）

8. 视频裁剪（待开发）

**当前项目只是实现了整体主要功能，用户交互方面还是比较粗糙，性能也没有专门进行优化过，以后应该还会不断优化和扩展功能~~**



**如果觉得不错，记得给颗小星星哦~**



A video and audio player
