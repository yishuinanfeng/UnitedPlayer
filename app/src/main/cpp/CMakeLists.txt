# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
#设置头文件路径（相对本文件路径）
include_directories(include)

#设置ffmpeg库所在路径的变量
set(FF ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI})
#设置ffmpeg的动态库，在链接的时候使用
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${FF}/libavcodec.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${FF}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${FF}/libavutil.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${FF}/libswscale.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${FF}/libswresample.so)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        IDemux.cpp
        FFDemux.cpp
        XData.cpp
        XLog.cpp
        XThread.cpp
        IObserver.cpp
        FFDecode.cpp
        IDecode.cpp
        XParameter.cpp
        GLVideoView.cpp
        IVideoView.cpp
        TextureHandler.cpp
        XEGL.cpp
        ShaderHandler.cpp
        IResample.cpp
        FFResample.cpp
        IAudioPlay.cpp
        SLAudioPlay.cpp
        IPlayer.cpp
        IPlayBuilder.cpp
        FFPlayBuilder.cpp
        IPlayerProxy.cpp
        Filter.cpp
        NoneFilter.cpp
        OppoFilter.cpp
        GrayFilter.cpp
        SplashFilter.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        GLESv2
        EGL
        android
        avcodec avformat avutil swscale swresample
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})