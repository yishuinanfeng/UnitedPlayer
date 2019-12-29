package com.haha.record.encodec

import android.content.Context
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.media.MediaMuxer
import android.util.Log
import android.view.Surface
import com.haha.record.egl.CustomGlSurfaceView
import com.haha.record.egl.EglHelper
import java.lang.ref.WeakReference
import javax.microedition.khronos.egl.EGLContext

//声道数
const val CHANNEL_COUNT = 2
//位深
const val BIT_DEPTH = 2

/**
 * 这里将OpenGL数据渲染到MediaCodec的输入Surface中
 */
open class BaseMediaEncoder(val context: Context) {

    private var onMediaInfoListener: OnMediaInfoListener? = null

    private var mSurface: Surface? = null
    private var mEglContext: EGLContext? = null
    private var mRender: CustomGlSurfaceView.CustomRender? = null

    private var mRenderMode = RENDERMODE_CONTINUOUSLY

    private var width: Int = 0
    private var height: Int = 0

    private var videoEncoder: MediaCodec? = null
    private var audioEncoder: MediaCodec? = null
    private var videoFormat: MediaFormat? = null
    private var audioFormat: MediaFormat? = null
    //Per buffer metadata includes an offset and size specifying
    // the range of valid data in the associated codec (output) buffer.
    private var videoBufferInfo: MediaCodec.BufferInfo? = null
    private var audioBufferInfo: MediaCodec.BufferInfo? = null

    private var mediaMuxer: MediaMuxer? = null

    private var eglMediaThread: EglMediaThread? = null
    private var videoEncodeThread: VideoEncodeThread? = null
    private var audioEncodeThread: AudioEncodeThread? = null

    private var audioPts: Long = 0
    private var sampleRate: Int = 0

    //todo 这里使用一三标志位来进行音视频封装的同步（可以用wait notify进行同步？）
    @Volatile
    private var encodeStart = false
    @Volatile
    private var audioExit = false
    @Volatile
    private var videoExit = false

    companion object {
        private val TAG = BaseMediaEncoder::class.java.simpleName
        const val RENDERMODE_WHEN_DIRTY = 0
        const val RENDERMODE_CONTINUOUSLY = 1
    }

    fun setRender(render: CustomGlSurfaceView.CustomRender) {
        this.mRender = render
    }

    fun setRenderMode(mode: Int) {
        if (mRender == null) {
            throw java.lang.RuntimeException("must set render before set render mode!")
        }
        mRender?.let {
            mRenderMode = mode
        }
    }

    fun startRecord() {
        audioExit = false
        videoExit = false
        encodeStart = false
        audioPts = 0

        eglMediaThread = EglMediaThread(WeakReference(this))
        videoEncodeThread = VideoEncodeThread(WeakReference(this))
        audioEncodeThread = AudioEncodeThread(WeakReference(this))
        eglMediaThread!!.isCreate = true
        eglMediaThread!!.isChange = true
        eglMediaThread!!.start()
        videoEncodeThread!!.start()
        audioEncodeThread!!.start()
    }

    fun stopRecord() {
        videoEncodeThread?.exit()
        audioEncodeThread?.exit()
        eglMediaThread?.onDestroy()

        videoEncodeThread = null
        audioEncodeThread = null
        eglMediaThread = null
    }

    /**
     * 传入PCM数据给MediaCodec
     * @param buffer：pcm数据
     * @param size：pcm数据大小
     */
    fun putPcmData(buffer: ByteArray, size: Int) {
        if (audioEncodeThread != null && !audioEncodeThread!!.isExit && size > 0) {
            audioEncoder?.let {
                val inputBufferIndex = it.dequeueInputBuffer(0)
                if (inputBufferIndex >= 0) {
                    val byteBuffer = it.inputBuffers[inputBufferIndex]
                    byteBuffer.clear()
                    byteBuffer.put(buffer)
                    val pst = getAudioPts(size, sampleRate)
                    it.queueInputBuffer(inputBufferIndex, 0, size, pst, 0)
                }
            }

        }
    }

    /**
     * 获取当前音频帧的pst
     * @param size:当前音频帧的大小
     * @param sampleRate:采样率
     */
    private fun getAudioPts(size: Int, sampleRate: Int): Long {
        audioPts += ((1.0 * size / (sampleRate * CHANNEL_COUNT * BIT_DEPTH)) * 1000000.0).toLong()
        Log.d("getAudioPts", "getAudioPts:$audioPts")
        return audioPts
    }

    fun initEncoder(
        eglContext: EGLContext,
        savePath: String,
        width: Int,
        height: Int,
        sampleRate: Int
    ) {
        this.width = width
        this.height = height
        this.mEglContext = eglContext
        initMediaEncoder(savePath, width, height, sampleRate)
    }

    private fun initMediaEncoder(savePath: String, width: Int, height: Int, sampleRate: Int) {
        //MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4表示封装格式为mp4
        mediaMuxer = MediaMuxer(savePath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
        initVideoEncoder(MediaFormat.MIMETYPE_VIDEO_AVC, width, height)
        initAudioEncoder(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, CHANNEL_COUNT)
    }

    /**
     * 初始化视频解码器
     * @param mimeType:编码格式
     */
    private fun initVideoEncoder(mimeType: String, width: Int, height: Int) {
        videoBufferInfo = MediaCodec.BufferInfo()
        videoFormat = MediaFormat.createVideoFormat(mimeType, width, height)
        //MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface常量，设置MediaFormat.KEY_COLOR_FORMAT后就可以通过
        // mEncoder.createInputSurface()创建一个Surface，然后你就可以通过OpenGL将图像绘制到这个Surface上，MediaCodec就可以通过这个Surface录制出H264
        videoFormat!!.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface)
        //设置码率（设置是否正确？这里只是设置了一帧的大小？？）
        videoFormat!!.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4)
        //设置帧率（一秒30帧）
        videoFormat!!.setInteger(MediaFormat.KEY_FRAME_RATE, 30)
        //设置关键帧的间隔为1秒
        videoFormat!!.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 10)
        //根据传入的类型得到对应的编码器
        videoEncoder = MediaCodec.createEncoderByType(mimeType)
        //surface为解码输出surface，这里只需要录制所以没有。
        // CONFIGURE_FLAG_ENCODE表示配置为编码器
        //configure之后从Unitialized进入Configured状态
        videoEncoder!!.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        //mSurface给openGL渲染Camera预览数据作为MediaCodec的输入
        mSurface = videoEncoder!!.createInputSurface()
    }

    private fun initAudioEncoder(mimeType: String, sampleRate: Int, channelCount: Int) {
        this.sampleRate = sampleRate
        audioBufferInfo = MediaCodec.BufferInfo()
        audioFormat = MediaFormat.createAudioFormat(mimeType, sampleRate, channelCount)
        audioFormat!!.setInteger(MediaFormat.KEY_BIT_RATE, 96000)
        audioFormat!!.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC)
        //缓冲最大大小
        audioFormat!!.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096)

        audioEncoder = MediaCodec.createEncoderByType(mimeType)
        audioEncoder!!.configure(audioFormat!!, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        //MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface常量，设置MediaFormat.KEY_COLOR_FORMAT后就可以通过
        // mEncoder.createInputSurface()创建一个Surface，然后你就可以通过OpenGL将图像绘制到这个Surface上，MediaCodec就可以通过这个Surface录制出H264

    }

    /**
     * OpenGL渲染线程，向MediaCodec的输入Surface提供原始视频数据
     * 可以看作VideoEncodeThread的生产者
     */
    class EglMediaThread(private val encodeReference: WeakReference<BaseMediaEncoder>) : Thread() {

        private var eglHelper: EglHelper? = null

        private var isExit = false
        private var isStart = false
        private var lock: Object? = null

        var isChange = false
        var isCreate = false

        override fun run() {
            isExit = false
            isStart = false
            eglHelper = EglHelper()
            lock = Object()
            encodeReference.get()?.let {
                if (it.mSurface == null) {
                    return
                }
                //该surface为MediaCodec的输入Surface，这里就将egl和MediaCodec绑定，使得渲染上去的数据作为编码的输入
                eglHelper!!.initEgl(it.mSurface!!, it.mEglContext)
            }

            while (true) {
                if (isExit) {
                    release()
                    break
                }

                //已经开始才进行画面刷新
                if (isStart) {
                    encodeReference.get()?.let {
                        when {
                            it.mRenderMode == RENDERMODE_WHEN_DIRTY -> synchronized(lock!!) {
                                //等待手动刷新
                                lock!!.wait()
                            }
                            it.mRenderMode == RENDERMODE_CONTINUOUSLY -> sleep(60)
                            else -> throw RuntimeException("mRenderMode is wrong value")
                        }
                    }
                }

                onCreate()
                onChange(encodeReference.get()?.width, encodeReference.get()?.height)
                onDraw()

                isStart = true
            }
        }

        private fun onDraw() {
            encodeReference.get()?.let {
                if (it.mRender != null && eglHelper != null) {
                    it.mRender!!.onDrawFrame()
                    if (!isStart) {
                        it.mRender!!.onDrawFrame()
                    }

                    eglHelper!!.swapBuffers()
                }
            }
        }


        /**
         * 手动刷新
         */
        fun requestRender() {
            lock?.let {
                synchronized(it) {
                    it.notifyAll()
                }
            }
        }

        fun onDestroy() {
            isExit = true
            //??
            requestRender()
        }

        fun getEglContext(): EGLContext? {
            return eglHelper?.getEglContext()
        }

        private fun onChange(width: Int?, height: Int?) {
            if (width == null || height == null) {
                return
            }
            encodeReference.get()?.let {
                if (isChange && it.mRender != null) {
                    isChange = false
                    it.mRender!!.onSurfaceChanged(width, height)
                }
            }

        }

        private fun onCreate() {
            encodeReference.get()?.let {
                if (isCreate && it.mRender != null) {
                    isCreate = false
                    it.mRender!!.onSurfaceCreated()
                }

            }
        }

        private fun release() {
            eglHelper?.destroyEgl()
            eglHelper = null
            lock = null
        }
    }

    /**
     * 视频编码线程。MediaCodec用Surface接收OepnGl渲染线程传来的原始视频帧，进行编码，编码后将视频帧交给MediaMuxer进行封装
     * 可以看作EglMediaThread的消费者
     */
    class VideoEncodeThread(private val mediaEncodeReference: WeakReference<BaseMediaEncoder>) : Thread() {

        var videoTrackIndex: Int? = -1

        private var videoEncoder: MediaCodec? = mediaEncodeReference.get()?.videoEncoder
        private var videoFormat: MediaFormat? = mediaEncodeReference.get()?.videoFormat
        private var videoBufferInfo: MediaCodec.BufferInfo? = mediaEncodeReference.get()?.videoBufferInfo
        private var mediaMuxer: MediaMuxer? = mediaEncodeReference.get()?.mediaMuxer
        @Volatile
        private var isExit = false
        private var pts: Long = 0

        override fun run() {
            isExit = false

            if (videoEncoder == null || videoFormat == null || videoBufferInfo == null || mediaMuxer == null) {
                return
            }
            //从Configured进入flushed状态
            videoEncoder?.start()
            while (true) {
                if (isExit) {
                    videoEncoder?.stop()
                    //进入Released状态
                    videoEncoder?.release()
                    videoEncoder = null
                    //MediaMuxer只有stop的时候才会将头信息写入
                    //保证音视频编码都退出了才停止mediaMuxer
                    mediaEncodeReference.get()?.videoExit = true
                    mediaEncodeReference.get()?.let {
                        if (it.audioExit) {
                            mediaMuxer?.stop()
                            mediaMuxer?.release()
                            mediaMuxer = null
                        }
                    }
                    Log.d(TAG, "videoEncoder stop")

                    break
                }

                //循环从MediaCodec中取出Buffer,如果是存储编码好的buffer则将编码好的buffer传入MediaMuxer中
                //As soon as the first input buffer is dequeued, the codec moves to the Running sub-state
                var outputBufferIndex = videoEncoder!!.dequeueOutputBuffer(videoBufferInfo!!, 0)
                Log.d(TAG, "videoEncoder outputBufferIndex：$outputBufferIndex")
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    //format改变 意味着要开始将得到的数据编码成mp4了，启动mMuxer
                    //生成一个视频轨道，提供给之后复用封装使用。既然是封装，就需要之si指定封装格式
                    Log.d(TAG, "addTrack videoEncoder!!.outputFormat:${videoEncoder!!.outputFormat}")
                    videoTrackIndex = mediaMuxer!!.addTrack(videoEncoder!!.outputFormat)
                    mediaEncodeReference.get()?.audioEncodeThread?.let {
                        //在音视频都拿到TrackIndex才可以开启mediaMuxer!!.start()
                        if (it.audioTrackIndex != -1) {
                            mediaMuxer!!.start()
                            mediaEncodeReference.get()!!.encodeStart = true
                        }
                    }

                } else {
                    //因为有时候一个buffer不够存放一帧数据，所以这里循环取连续的buffer
                    //outputBufferIndex为Buffer的索引，大于0说明是编码成功的Buffer？
                    while (outputBufferIndex >= 0) {
                        if (mediaEncodeReference.get()!!.encodeStart) {
                            val outputBuffer = videoEncoder!!.outputBuffers[outputBufferIndex]
                            //videoBufferInfo指定每个buffer中有效数据的偏移量和大小（疑问：不同的buffer使用同一个videoBufferInfo？？）
                            outputBuffer.position(videoBufferInfo!!.offset)
                            outputBuffer.limit(videoBufferInfo!!.offset + videoBufferInfo!!.size)
                            //设置时间戳从0开始，不设置的话视频的时间戳会错误，比如播放器播放的时候把时间长度搞错
                            //疑问：为什么dequeueOutputBuffer出来的videoBufferInfo中的时间戳并不是实际时间？
                            if (pts == 0L) {
                                pts = videoBufferInfo!!.presentationTimeUs
                            }

                            videoBufferInfo!!.presentationTimeUs = videoBufferInfo!!.presentationTimeUs - pts

                            //将输出Buffer（即已经编码好的视频帧数据）写入复用器（这里需要确保轨道index正确）
                            mediaMuxer!!.writeSampleData(videoTrackIndex!!, outputBuffer, videoBufferInfo!!)

                            mediaEncodeReference.get()
                                ?.onMediaInfoListener?.onMediaTime(videoBufferInfo!!.presentationTimeUs / 1000000)
                        }

                        //将用完的OutputBuffer会还给videoEncoder
                        videoEncoder!!.releaseOutputBuffer(outputBufferIndex, false)
                        //循环取出已经编码好的视频帧（注意：这里每次取出来的videoBufferInfo存储的就是新的帧的数据）
                        outputBufferIndex = videoEncoder!!.dequeueOutputBuffer(videoBufferInfo!!, 0)
                    }
                }
            }

        }

        fun exit() {
            isExit = true
        }
    }

    class AudioEncodeThread(private val mediaEncodeReference: WeakReference<BaseMediaEncoder>) : Thread() {
        var audioTrackIndex: Int = -1
        @Volatile
        var isExit: Boolean = false

        private var audioEncoder: MediaCodec? = mediaEncodeReference.get()?.audioEncoder
        private var audioBufferInfo: MediaCodec.BufferInfo? = mediaEncodeReference.get()?.audioBufferInfo
        private var mediaMuxer: MediaMuxer? = mediaEncodeReference.get()?.mediaMuxer
        private var pts: Long = 0

        override fun run() {
            pts = 0
            audioTrackIndex = -1
            isExit = false

            if (audioEncoder == null || audioBufferInfo == null || mediaMuxer == null) {
                return
            }

            audioEncoder?.start()
            while (true) {
                if (isExit) {

                    audioEncoder?.stop()
                    audioEncoder?.release()
                    audioEncoder = null
                    mediaEncodeReference.get()?.audioExit = true
                    mediaEncodeReference.get()?.let {
                        //保证音视频编码都退出了才停止mediaMuxer
                        if (it.videoExit) {
                            //MediaMuxer只有stop的时候才会将头信息写入
                            mediaMuxer?.stop()
                            mediaMuxer?.release()
                            mediaMuxer = null
                        }
                    }

                    break
                }
                //循环从MediaCodec中取出Buffer,如果是存储编码好的buffer则将编码好的buffer传入MediaMuxer中
                var outputBufferIndex = audioEncoder!!.dequeueOutputBuffer(audioBufferInfo!!, 0)
                Log.d(TAG, "audioEncoder outputBufferIndex：$outputBufferIndex")
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    audioTrackIndex = mediaMuxer!!.addTrack(audioEncoder!!.outputFormat)
                    mediaEncodeReference.get()?.videoEncodeThread?.let {
                        //在音视频都拿到TrackIndex才可以开启mediaMuxer!!.start()
                        if (it.videoTrackIndex != -1) {
                            mediaMuxer!!.start()
                            mediaEncodeReference.get()!!.encodeStart = true
                        }
                    }

                } else {
                    //因为有时候一个buffer不够存放一帧数据，所以这里循环取连续的buffer
                    while (outputBufferIndex >= 0) {
                        //mediaMuxer开启之后才开始编码
                        if (mediaEncodeReference.get()!!.encodeStart) {
                            val outputBuffer = audioEncoder!!.outputBuffers[outputBufferIndex]
                            //audioBufferInfo指定每个buffer中有效数据的偏移量和大小（疑问：不同的buffer使用同一个audioBufferInfo？？答：每次dequeueOutputBuffer都会更新该值）
                            //即找出其中的有效数据
                            outputBuffer.position(audioBufferInfo!!.offset)
                            outputBuffer.limit(audioBufferInfo!!.offset + audioBufferInfo!!.size)
                            //设置时间戳从0开始，不设置的话音频的时间戳会错误，比如播放器播放的时候把时间长度搞错
                            //疑问：为什么dequeueOutputBuffer出来的videoBufferInfo中的时间戳并不是实际时间？
                            if (pts == 0L) {
                                pts = audioBufferInfo!!.presentationTimeUs
                            }

                            audioBufferInfo!!.presentationTimeUs = audioBufferInfo!!.presentationTimeUs - pts

                            Log.d(
                                "audioBufferInfo",
                                "audioBufferInfo!!.presentationTimeUs:${audioBufferInfo!!.presentationTimeUs}"
                            )

                            //将输出Buffer（即已经编码好的音频帧数据）写入复用器（这里需要确保轨道index正确）
                            mediaMuxer!!.writeSampleData(audioTrackIndex, outputBuffer, audioBufferInfo!!)
                        }
                        audioEncoder!!.releaseOutputBuffer(outputBufferIndex, false)
                        outputBufferIndex = audioEncoder!!.dequeueOutputBuffer(audioBufferInfo!!, 0)
                    }
                }
            }
        }

        fun exit() {
            isExit = true
        }
    }


    interface OnMediaInfoListener {
        fun onMediaTime(times: Long)
    }

    fun setOnMediaInfoListener(onMediaInfoListener: OnMediaInfoListener) {
        this.onMediaInfoListener = onMediaInfoListener
    }
}