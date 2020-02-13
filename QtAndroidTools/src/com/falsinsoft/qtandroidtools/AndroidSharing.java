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
import android.os.Bundle;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;

import java.io.InputStream;
import java.io.IOException;
import java.io.FileNotFoundException;

public class AndroidSharing
{
    private final Activity mActivityInstance;
    private final Intent mActivityIntent;

    public AndroidSharing(Activity ActivityInstance)
    {
        mActivityInstance = ActivityInstance;
        mActivityIntent = ActivityInstance.getIntent();
    }

    public int getAction()
    {
        int ActionId = ACTION_NONE;

        switch(mActivityIntent.getAction())
        {
            case Intent.ACTION_SEND:
                ActionId = ACTION_SEND;
                break;
            case Intent.ACTION_SEND_MULTIPLE:
                ActionId = ACTION_SEND_MULTIPLE;
                break;
            case Intent.ACTION_PICK:
                ActionId = ACTION_PICK;
                break;
        }

        return ActionId;
    }

    public String getMimeType()
    {
        return mActivityIntent.getType();
    }

    public String getSharedText()
    {
        return mActivityIntent.getStringExtra(Intent.EXTRA_TEXT);
    }

    public byte[] getSharedData()
    {
        final Uri DataUri = (Uri)mActivityIntent.getParcelableExtra(Intent.EXTRA_STREAM);
        byte[] ByteArray = null;
        InputStream DataStream;

        try
        {
            DataStream = mActivityInstance.getContentResolver().openInputStream(DataUri);
            ByteArray = new byte[DataStream.available()];
            DataStream.read(ByteArray);
        }
        catch(FileNotFoundException e)
        {
            return null;
        }
        catch(IOException e)
        {
            return null;
        }

        return ByteArray;
    }

    public void shareText(String Text)
    {
        Intent SendIntent = new Intent();
        SendIntent.setAction(Intent.ACTION_SEND);
        SendIntent.putExtra(Intent.EXTRA_TEXT, Text);
        SendIntent.setType("text/plain");

        Intent ShareIntent = Intent.createChooser(SendIntent, null);
        mActivityInstance.startActivity(ShareIntent);
    }

    private int ACTION_NONE = 0;
    private int ACTION_SEND = 1;
    private int ACTION_SEND_MULTIPLE = 2;
    private int ACTION_PICK = 3;
}
