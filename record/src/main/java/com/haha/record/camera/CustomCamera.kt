package com.haha.record.camera

import android.content.Context
import android.graphics.ImageFormat
import android.graphics.SurfaceTexture
import android.hardware.Camera
import android.util.Log
import com.haha.record.util.DisplayUtil


class CustomCamera(private val context: Context) {

    private val TAG = this::class.java.simpleName

    private var width: Int = 0
    private var height: Int = 0
    private var surfaceTexture:SurfaceTexture? = null
    private var camera:Camera? = null

    init {
        this.width = DisplayUtil.getScreenWidth(context)
        this.height = DisplayUtil.getScreenHeight(context)
    }

    fun initCamera(surfaceTexture: SurfaceTexture,cameraId:Int){
        this.surfaceTexture = surfaceTexture
        setCameraParam(cameraId)
    }

    private fun setCameraParam(cameraId: Int) {
        Log.e(TAG, "setCameraParam")
        camera = Camera.open(cameraId)
        //surfaceTexture和OpenGL的纹理绑定好了，这里和Camera绑定，使得Camera的预览数据渲染到OpenGL纹理
        camera!!.setPreviewTexture(surfaceTexture)
        val parameter = camera!!.parameters

        parameter.flashMode = "off"
        parameter.previewFormat = ImageFormat.NV21
        parameter.setRecordingHint(true)

        val cameraSupportSize = getFitSize(parameter.supportedPictureSizes)
        parameter.setPictureSize(cameraSupportSize.width,cameraSupportSize.height)
        Log.e(TAG,"PictureSize width:${cameraSupportSize.width},height:${cameraSupportSize.height}")

        val cameraPreviewSize = getFitSize(parameter.supportedPreviewSizes)
        parameter.setPreviewSize(cameraPreviewSize.width,cameraPreviewSize.height)
        Log.e(TAG,"setPreviewSize width:${cameraPreviewSize.width},height:${cameraPreviewSize.height}")

        camera!!.parameters = parameter
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
        if (width < height) {
            val t = height
            height = width
            width = t
        }

        //找到Camera中预览尺寸比例和屏幕比例一致的尺寸
        for (size in sizes) {
            if (1.0f * size.width / size.height == 1.0f * width / height) {
                return size
            }
        }
        //找不到比例一致的则返回第一个尺寸
        return sizes[0]
    }
}