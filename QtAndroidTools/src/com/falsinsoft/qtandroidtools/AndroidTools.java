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

public class AndroidTools
{
    private static final String TAG = "AndroidTools";
    private final Activity mActivityInstance;

    public AndroidTools(Context context)
    {
        mActivityInstance = (Activity)context;
    }

    public int getActivityAction()
    {
        final String actionValue = mActivityInstance.getIntent().getAction();
        int actionId = ACTION_NONE;

        if(actionValue != null)
        {
            switch(actionValue)
            {
                case Intent.ACTION_SEND:
                    actionId = ACTION_SEND;
                    break;
                case Intent.ACTION_SEND_MULTIPLE:
                    actionId = ACTION_SEND_MULTIPLE;
                    break;
                case Intent.ACTION_PICK:
                    actionId = ACTION_PICK;
                    break;
            }
        }

        return actionId;
    }

    public String getActivityMimeType()
    {
        return mActivityInstance.getIntent().getType();
    }

    private int ACTION_NONE = 0;
    private int ACTION_SEND = 1;
    private int ACTION_SEND_MULTIPLE = 2;
    private int ACTION_PICK = 3;
}
