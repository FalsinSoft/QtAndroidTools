/*
 *	MIT License
 *
 *	Copyright (c) 2018 Fabio Falsini <falsinsoft@gmail.com>
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

package com.falsinsoft.qtandroidtools;

import android.content.Context;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.view.WindowManager;
import android.view.Window;
import android.view.View;
import android.util.Log;

public class AndroidScreen
{
    private static final String TAG = "AndroidScreen";
    private final Activity mActivityInstance;

    public AndroidScreen(Activity activityInstance)
    {
        mActivityInstance = activityInstance;
    }

    public void setOrientation(int orientation)
    {
        final int requestedOrientation;

        switch(orientation)
        {
            case SCREEN_ORIENTATION_LANDSCAPE:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
                break;
            case SCREEN_ORIENTATION_REVERSE_LANDSCAPE:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
                break;
            case SCREEN_ORIENTATION_SENSOR_LANDSCAPE:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE;
                break;
            case SCREEN_ORIENTATION_PORTRAIT:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
                break;
            case SCREEN_ORIENTATION_REVERSE_PORTRAIT:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
                break;
            case SCREEN_ORIENTATION_SENSOR_PORTRAIT:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT;
                break;
            default:
                requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED;
                break;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mActivityInstance.setRequestedOrientation(requestedOrientation);
            }
        });
    }

    public void keepScreenOn(final boolean keepOn)
    {
        final View contentView = mActivityInstance.getWindow().getDecorView().findViewById(android.R.id.content);

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                contentView.setKeepScreenOn(keepOn);
            }
        });
    }

    private final int SCREEN_ORIENTATION_LANDSCAPE = 0;
    private final int SCREEN_ORIENTATION_REVERSE_LANDSCAPE = 1;
    private final int SCREEN_ORIENTATION_SENSOR_LANDSCAPE = 2;
    private final int SCREEN_ORIENTATION_PORTRAIT = 3;
    private final int SCREEN_ORIENTATION_REVERSE_PORTRAIT = 4;
    private final int SCREEN_ORIENTATION_SENSOR_PORTRAIT = 5;
}
