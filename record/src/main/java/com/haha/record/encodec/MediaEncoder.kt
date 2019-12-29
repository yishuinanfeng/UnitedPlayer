package com.haha.record.encodec

import android.content.Context

class MediaEncoder(context: Context,textureId:Int): BaseMediaEncoder(context) {

    private val mediaEncodeRender: MediaEncodeRender = MediaEncodeRender(context,textureId)

    init {
        setRender(mediaEncodeRender)
        setRenderMode(RENDERMODE_CONTINUOUSLY)
    }
}