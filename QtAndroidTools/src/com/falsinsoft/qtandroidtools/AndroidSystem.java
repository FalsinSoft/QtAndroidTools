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
 import android.net.Uri;
 import android.util.Log;
 import android.util.TypedValue;

 public class AndroidSystem
 {
    private static final String TAG = "AndroidSystem";
    private final Activity mActivityInstance;

    public AndroidSystem(Activity activityInstance)
    {
        mActivityInstance = activityInstance;
    }
	
    public int spToPx(float sp)
    {
        return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_SP, sp, mActivityInstance.getResources().getDisplayMetrics());
    }

    public int dipToPx(float dip)
    {
        return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dip, mActivityInstance.getResources().getDisplayMetrics());
    }

    public int ptToPx(float pt)
    {
        return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_PT, pt, mActivityInstance.getResources().getDisplayMetrics());
    }
}
