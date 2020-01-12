package com.haha.record

import android.os.Environment

fun getVideoParentPath(): String? {
    if (Environment.getExternalStorageState() != Environment.MEDIA_MOUNTED) {
        return null
    }
    return "${Environment.getExternalStorageDirectory().path}/unitedPlayer"
}