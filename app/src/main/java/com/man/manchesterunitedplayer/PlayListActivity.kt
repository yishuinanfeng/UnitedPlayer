package com.man.manchesterunitedplayer

import android.app.Activity
import android.os.Bundle

enum class FilterType(val filterType: Int) {
    OPPOSITE_COLOR(1),//反色
    GRAY(2), //灰度图
}

class PlayListActivity : Activity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }
}