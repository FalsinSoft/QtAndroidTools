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
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;

import java.util.List;

public class AndroidPlayStore
{
    private final Activity mActivityInstance;

    public AndroidPlayStore(Activity ActivityInstance)
    {
        mActivityInstance = ActivityInstance;
    }

    public void open(String params)
    {
        Intent PlayStoreIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("market://" + params));
        final List<ResolveInfo> AppInfoList = mActivityInstance.getPackageManager().queryIntentActivities(PlayStoreIntent, 0);
        boolean PlayStoreFound = false;

        for(ResolveInfo AppInfo: AppInfoList)
        {
            if(AppInfo.activityInfo.applicationInfo.packageName.equals("com.android.vending"))
            {
                PlayStoreFound = true;

                PlayStoreIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED | Intent.FLAG_ACTIVITY_CLEAR_TOP);
                PlayStoreIntent.setComponent(new ComponentName(AppInfo.activityInfo.applicationInfo.packageName, AppInfo.activityInfo.name));

                mActivityInstance.startActivity(PlayStoreIntent);
                break;
            }
        }

        if(PlayStoreFound == false)
        {
            Intent WebPlayStoreIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://play.google.com/store/apps/" + params));
            mActivityInstance.startActivity(WebPlayStoreIntent);
        }
    }
}
