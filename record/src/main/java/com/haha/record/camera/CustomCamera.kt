package com.haha.record.camera

import android.content.Context
import android.graphics.ImageFormat
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.util.Log
import com.haha.record.util.DisplayUtil

/**
 * Camera包装类，包括对Camera的配置的预览启动、停止
 */
class CustomCamera(context: Context) {

    private val TAG = this::class.java.simpleName

    private var deviceWidth: Int = 0
    private var deviceHeight: Int = 0
    private var surfaceTexture:SurfaceTexture? = null
    private var camera:Camera? = null

    init {
        //设备的宽高
        this.deviceWidth = DisplayUtil.getScreenWidth(context)
        this.deviceHeight = DisplayUtil.getScreenHeight(context)
    }

    fun initCamera(surfaceTexture: SurfaceTexture,cameraId:Int){
        this.surfaceTexture = surfaceTexture
        setCameraParam(cameraId)
    }

    private fun setCameraParam(cameraId: Int) {
        Log.e(TAG, "setCameraParam")
        //获取一个Camera对象
        // You must call {@link #release()} when you are done using the camera,
        // otherwise it will remain locked and be unavailable to other applications
        camera = Camera.open(cameraId)
        //surfaceTexture和OpenGL的纹理绑定好了，这里surfaceTexture和Camera绑定作为Camera数据的输出端
        //使得Camera的预览数据渲染到OpenGL纹理
        camera!!.setPreviewTexture(surfaceTexture)
        val parameter = camera!!.parameters
        //闪光灯
        parameter.flashMode = "off"
        //预览数据格式
        parameter.previewFormat = ImageFormat.NV21
        //提升录制视频性能用的（主要提升MediaRecorder录制的时候，这里其他应该并没有用处）
        parameter.setRecordingHint(true)
        //持续对焦
        parameter.focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO

        //以下分别设置摄像头的拍照图片和预览分辨率，因为默认的分辨率并非最清晰合适的，所以必须手动选择最佳的
        //通过屏幕宽高比找到最合适的摄像头拍照图片尺寸
        val cameraSupportSize = getFitSize(parameter.supportedPictureSizes)
        parameter.setPictureSize(cameraSupportSize.width,cameraSupportSize.height)
        Log.e(TAG,"PictureSize width:${cameraSupportSize.width},height:${cameraSupportSize.height}")
        //通过屏幕宽高比找到最合适的摄像头预览尺寸
        val cameraPreviewSize = getFitSize(parameter.supportedPreviewSizes)
        //设置摄像头的预览尺寸
        parameter.setPreviewSize(cameraPreviewSize.width,cameraPreviewSize.height)
        Log.e(TAG,"setPreviewSize width:${cameraPreviewSize.width},height:${cameraPreviewSize.height}")
        //设置所有的摄像头参数
        camera!!.parameters = parameter
        //开始摄像头预览
        camera!!.startPreview()
    }

    /**
     * 释放摄像头之后，其他进程才可以使用摄像头
     */
    fun stopPreview(){
        Log.e(TAG, "stopPreview")
        camera?.stopPreview()
        camera?.release()
        camera = null
    }

    /**
     * 切换摄像头
     */
    fun changeCamera(cameraId: Int){
        stopPreview()
        setCameraParam(cameraId)
    }

    /**
     * 找到Camera中预览尺寸比例和屏幕比例一致的尺寸
     */
    private fun getFitSize(sizes: List<Camera.Size>): Camera.Size {
        //因为摄像头预览都是横屏的（宽大于高），所以这里如果宽小于高则调换下
        if (deviceWidth < deviceHeight) {
            val t = deviceHeight
            deviceHeight = deviceWidth
            deviceWidth = t
        }

        //找到Camera中预览尺寸比例和屏幕比例一致的尺寸
        for (size in sizes) {
            if (1.0f * size.width / size.height == 1.0f * deviceWidth / deviceHeight) {
                return size
            }
        }
        //找不到比例一致的则返回第一个尺寸
        return sizes[0]
    }
}