package com.man.manchesterunitedplayer.playlist

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import com.haha.record.getVideoParentPath
import java.io.File
import android.media.MediaMetadataRetriever
import android.media.ThumbnailUtils
import android.provider.MediaStore
import android.util.Log
import androidx.recyclerview.widget.LinearLayoutManager
import com.man.manchesterunitedplayer.KEY_INTENT_VIDEO_PATH
import com.man.manchesterunitedplayer.PlayActivity
import com.man.manchesterunitedplayer.R
import kotlinx.android.synthetic.main.activity_play_list.*
import java.io.FileOutputStream


enum class FilterType(val filterType: Int) {
    OPPOSITE_COLOR(1),//反色
    GRAY(2), //灰度图
}

class PlayListActivity : Activity() {

    //  private val mediaMetadataRetriever = MediaMetadataRetriever()
    private val mVideoList = mutableListOf<VideoBean>()
    private val adapter: PlayListAdapter by lazy {
        val onItemClick: (String) -> Unit = { path ->
            Intent(this, PlayActivity::class.java)
                    .apply {
                        putExtra(KEY_INTENT_VIDEO_PATH, path)
                    }
                    .apply {
                        startActivity(this)
                    }
        }
        val playListAdapter = PlayListAdapter(mVideoList, onItemClick)
        playListAdapter
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_play_list)
        rv_video_list.layoutManager = LinearLayoutManager(this)
        rv_video_list.adapter = adapter

        Thread {
            getVideoList()?.let {
                runOnUiThread {
                    mVideoList.addAll(it)
                    adapter.notifyDataSetChanged()
                }
            }
        }.start()
    }

    private fun getVideoList(): MutableList<VideoBean>? {
        val videoParent = getVideoParentPath() ?: return null
        Log.d("PlayListActivity", "videoParent:$videoParent")
        val videoParentFile = File(videoParent)
        if (!videoParentFile.exists()) {
            videoParentFile.mkdirs()
        }
        val videoList = mutableListOf<VideoBean>()
        val fs = videoParentFile.listFiles() ?: return null
        for (f in fs) {
            if (!f.isDirectory) {
                Log.d("PlayListActivity", "getVideoThumb f.path:${f.path}")
                val bitmapThumb = getVideoThumb(f.path)
                val videoBean = VideoBean(f.path, bitmapThumb, f.name)
                Log.d("PlayListActivity", "videoBean:${videoBean}")
                videoList.add(videoBean)
            }
        }

        return videoList
    }

    private fun getVideoThumb(videoPath: String): Bitmap {
        return ThumbnailUtils.createVideoThumbnail(videoPath, MediaStore.Images.Thumbnails.MICRO_KIND)
    }
}