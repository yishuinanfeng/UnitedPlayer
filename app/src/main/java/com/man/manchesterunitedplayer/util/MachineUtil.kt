package com.man.manchesterunitedplayer.util

import android.content.Context
import android.graphics.Point
import android.view.WindowManager

/**
 * @return 设备高度
 */
fun getScreenHeight(context: Context): Int {
    val manager = context.getSystemService(Context.WINDOW_SERVICE) as WindowManager
    val display = manager.defaultDisplay
    val p = Point()
    display.getRealSize(p)
    return p.y
}

/**
 * @return 设备宽度
 */
fun getScreenWidth(context: Context): Int {
    val manager = context.getSystemService(Context.WINDOW_SERVICE) as WindowManager
    val display = manager.defaultDisplay
    val p = Point()
    display.getRealSize(p)
    return p.x
}