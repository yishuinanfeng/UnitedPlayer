package com.man.manchesterunitedplayer.playlist

import android.content.Intent
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import android.view.LayoutInflater
import android.widget.ImageView
import android.widget.TextView
import com.man.manchesterunitedplayer.R

/**
 * 播放列表Adapter
 */
class PlayListAdapter(private val videoList: List<VideoBean>, private val onItemClick: ((String) -> Unit)? = null) : RecyclerView.Adapter<PlayListAdapter.PlayVideoHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): PlayVideoHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.item_play_list, parent, false)
        return PlayVideoHolder(view)
    }

    override fun getItemCount(): Int {
        return videoList.size
    }

    override fun onBindViewHolder(holder: PlayVideoHolder, position: Int) {
        val videoBean = videoList[position]
        holder.imgVideoThumb.setImageBitmap(videoBean.videoThumb)
        holder.tvVideoName.text = videoBean.videoName
        holder.itemView.setOnClickListener {
            onItemClick?.invoke(videoBean.filePath)
        }
    }

    class PlayVideoHolder(val view: View) : RecyclerView.ViewHolder(view) {

        val tvVideoName = view.findViewById<TextView>(R.id.tv_video_name)
        val imgVideoThumb = view.findViewById<ImageView>(R.id.video_thumb)

    }
}

