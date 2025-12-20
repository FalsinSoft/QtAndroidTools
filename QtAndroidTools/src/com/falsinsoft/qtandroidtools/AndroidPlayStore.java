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
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.gms.tasks.Task;

import java.util.List;

public class AndroidPlayStore
{
    private final Activity mActivityInstance;

    public AndroidPlayStore(Context context)
    {
        mActivityInstance = (Activity)context;
    }

    public void open(String params)
    {
        Intent playStoreIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("market://" + params));
        final List<ResolveInfo> appInfoList = mActivityInstance.getPackageManager().queryIntentActivities(playStoreIntent, 0);
        boolean playStoreFound = false;

        for(ResolveInfo appInfo : appInfoList)
        {
            if(appInfo.activityInfo.applicationInfo.packageName.equals("com.android.vending"))
            {
                playStoreFound = true;

                playStoreIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED | Intent.FLAG_ACTIVITY_CLEAR_TOP);
                playStoreIntent.setComponent(new ComponentName(appInfo.activityInfo.applicationInfo.packageName, appInfo.activityInfo.name));

                mActivityInstance.startActivity(playStoreIntent);
                break;
            }
        }

        if(playStoreFound == false)
        {
            Intent webPlayStoreIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://play.google.com/store/apps/" + params));
            mActivityInstance.startActivity(webPlayStoreIntent);
        }
    }

    public void requestReview()
    {
        final ReviewManager manager = ReviewManagerFactory.create(mActivityInstance);
        final Task<ReviewInfo> request = manager.requestReviewFlow();

        request.addOnCompleteListener(task ->
        {
            if(task.isSuccessful())
            {
                final ReviewInfo reviewInfo = task.getResult();
                final Task<Void> flow = manager.launchReviewFlow(mActivityInstance, reviewInfo);

                flow.addOnCompleteListener(flowTask ->
                {
                    reviewRequestCompleted(true);
                });
            }
            else
            {
                reviewRequestCompleted(false);
            }
        });
    }

    private static native void reviewRequestCompleted(boolean successful);
}
