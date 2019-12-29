package com.haha.record.util;

import android.content.Context;
import android.util.DisplayMetrics;

public class DisplayUtil {

    public static int getScreenWidth(Context context)
    {
        DisplayMetrics metric = context.getResources().getDisplayMetrics();
        return metric.widthPixels;
    }

    public static int getScreenHeight(Context context)
    {
        DisplayMetrics metric = context.getResources().getDisplayMetrics();
        return metric.heightPixels;
    }

}
