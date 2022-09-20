# UnitedPlayer

**一个支持录制和播放音频视频的安卓短视频应用，录制的视频将在播放列表中显示，点击列表项即可播放，并可以选择多种仿抖音滤镜特效展示。**

博客链接：[介绍一个自己刚出炉的音视频播放录制开源项目](https://blog.csdn.net/sinat_23092639/article/details/104142700)

**相关博客系列目录：**  

[音视频开发系统学习的浪漫马车之总目录](https://juejin.cn/post/7033711226827833351)


**C内存与指针：**  
[漫谈C语言内存管理](https://juejin.cn/post/7033711811006464030)  
[漫谈C语言指针（一）](https://juejin.cn/post/7033711907660005413)  
[漫谈C语言指针（二）](https://juejin.cn/post/7033713132178669582)  
[漫谈C语言指针（三）](https://juejin.cn/post/7033713772011192333)  

**C++语法重点难点：**  
[初尝C++的世界](https://juejin.cn/post/7033714154880008222)  
[进一步走进C++面向对象的世界](https://juejin.cn/post/7033715596348882974)  
[感受C++一些令人眼前一亮的语法](https://juejin.cn/post/7035447100464693256)  

**C/C++进阶：**  
[C/C++编译流程](https://juejin.cn/post/7043778638802518052)  
[一篇文章入门C/C++自动构建利器之Makefile](https://juejin.cn/post/7046401218331017224)  
[升级构建工具，从Makefile到CMake](https://juejin.cn/post/7058217745321558024)  
[初探ndk的世界（一）](https://juejin.cn/post/7061553507622453279)  
[初探ndk的世界（二）](https://juejin.cn/user/2541726614684216)


**视频理论基础：**\
[视频基础知识扫盲](<https://juejin.cn/post/7082945332945879071>)\
[音视频开发基础知识之YUV颜色编码](<https://juejin.cn/post/7083427060240089096>)\
[解析视频编码原理——从孙艺珍的电影说起（一）](<https://juejin.cn/post/7083679329183334407>)\
[解析视频编码原理——从孙艺珍的电影说起（二）](<https://juejin.cn/post/7084062884983996430>)  
[H264码流结构一探究竟](https://juejin.cn/post/7092773284000989191/ "https://juejin.cn/post/7092773284000989191/")

**Android平台MediaCodec系列：**\
[Android硬编解码利器MediaCodec解析——从猪肉餐馆的故事讲起（一）](https://juejin.cn/post/7110805190512967717/ "https://juejin.cn/post/7110805190512967717/")\
[Android硬编解码工具MediaCodec解析——从猪肉餐馆的故事讲起（二）](https://juejin.cn/post/7111340889691127815/ "https://juejin.cn/post/7111340889691127815/")\
[Android硬编解码工具MediaCodec解析——从猪肉餐馆的故事讲起（三）](https://juejin.cn/post/7113767096512675870 "https://juejin.cn/post/7113767096512675870")

**轻松入门OpenGL系列**\
[一看就懂的OpenGL ES教程——图形渲染管线的那些事](https://juejin.cn/user/2541726614684216/posts "https://juejin.cn/user/2541726614684216/posts")\
[一看就懂的OpenGL ES教程——再谈OpenGL工作机制](https://juejin.cn/post/7121525553491869703 "https://juejin.cn/post/7121525553491869703")  
[一看就懂的OpenGL ES教程——这或许是你遇过最难画的三角形（一）](https://juejin.cn/post/7125449091563126792)  
[一看就懂的OpenGL ES教程——这或许是你遇过最难画的三角形（二）](https://juejin.cn/post/7134356782452834334/ "https://juejin.cn/post/7134356782452834334/")\
[一看就懂的OpenGL ES教程——这或许是你遇过最难画的三角形（三）](https://juejin.cn/post/7143614036046217230 "https://juejin.cn/post/7143614036046217230")  
[一看就懂的OpenGL ES教程——这或许是你遇过最难画的三角形（四）](https://juejin.cn/post/7144335420644392991)


## 基本技术实现点：

使用Camera+OpenGL ES实时预览画面，使用OpenSl ES录制音频，通过MediaCodec分别对音视频进行编码，并通过MediaMuxer将音视频封装为MP4格式。

使用ffmpeg解复用、解码MP4文件，通过OpenGL ES渲染视频画面，OpenSl ES播放音频，并通过OpenGL片段着色器提供不同的视频仿抖音滤镜效果。

## 功能点：

### 视频音频播放，支持进度条控制和暂停。（已完成） 

![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/playProgress.jpg)
 

### 视频音频录制。（已完成）

![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/record.jpg)

### 音频视频同步。（已完成）

### 视频尺寸自适应。（已完成）

### 视频添加水印。（已经实现，待扩展）

    

### 视频添加仿抖音滤镜。（已经实现最基础的滤镜：反色、灰度图、闪白、缩放、灵魂出窍、抖动）

  ![img](https://github.com/yishuinanfeng/UnitedPlayer/blob/master/gif/play.jpg)


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
   

### 视频添加背景音乐。（待开发）

### 音频变调。（待开发）

### 视频断点续录。（待开发）

### 视频裁剪（待开发）

**当前项目只是实现了整体主要功能，用户交互方面还是比较粗糙，性能也没有专门进行优化过，以后应该还会不断优化和扩展功能~~**



**如果觉得不错，记得给颗小星星哦~**



A video and audio player
