# UnitedPlayer

**一个支持录制和播放音频视频的应用，录制的视频将在播放列表中显示，点击列表项即可播放，并可以选择不同视频滤镜特效。**

## 基本技术实现点：

使用Camera+OpenGL ES实时预览画面，使用OpenSl ES录制音频，通过MediaCodec将音视频编码并封装为MP4格式。

使用ffmpeg解复用、解码MP4文件，通过OpenGL ES渲染视频画面，OpenSl ES播放音频，并通过OpenGL片段着色器提供不同的视频仿抖音滤镜效果。

## 功能点：

1. 视频音频播放，支持进度条控制和暂停。（已完成）  

2. 视频音频录制。（已完成）

3. 视频添加水印。（已经实现，待扩展）

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
   
   6. 抖动：
   
   ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/shake.gif)
   

5. 视频添加背景音乐。（待开发）

6. 音频变调。（待开发）

7. 视频断点续录。（待开发）

8. 视频裁剪（待开发）



**如果觉得不错，记得给颗小星星哦~**



A video and audio player
