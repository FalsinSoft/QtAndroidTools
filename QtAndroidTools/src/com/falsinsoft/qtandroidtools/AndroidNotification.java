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

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.os.Build;
import android.graphics.Bitmap;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.support.v4.app.NotificationManagerCompat;
import android.support.v4.app.NotificationCompat;

public class AndroidNotification
{
    private NotificationCompat.Builder mAppNotification;
    private boolean mNotificationEnabled = false;
    private final String NOTIFICATION_CHANNEL_ID;
    private final Activity mActivityInstance;
    private final int mNotificationId;

    public AndroidNotification(Activity ActivityInstance, int InstanceId)
    {
        NOTIFICATION_CHANNEL_ID = (ActivityInstance.getClass().getName() + Integer.toString(InstanceId));
        mAppNotification = new NotificationCompat.Builder(ActivityInstance, NOTIFICATION_CHANNEL_ID);
        mActivityInstance = ActivityInstance;
        mNotificationId = InstanceId;
    }

    public void show()
    {
        NotificationManagerCompat Manager = NotificationManagerCompat.from(mActivityInstance);
        mAppNotification.setPriority(NotificationCompat.PRIORITY_DEFAULT);
        Manager.notify(mNotificationId, mAppNotification.build());
        mNotificationEnabled = true;
    }

    public void cancel()
    {
        NotificationManagerCompat Manager = NotificationManagerCompat.from(mActivityInstance);
        Manager.cancel(mNotificationId);
        mNotificationEnabled = false;
    }

    private void update()
    {
        if(mNotificationEnabled == true)
        {
            NotificationManagerCompat Manager = NotificationManagerCompat.from(mActivityInstance);
            Manager.notify(mNotificationId, mAppNotification.build());
        }
    }

    public void setTitle(String title)
    {
        mAppNotification.setContentTitle(title);
        update();
    }

    public void setContent(String content)
    {
        mAppNotification.setContentText(content);
        update();
    }

    public void setExpandableContent(String expandableContent)
    {
        mAppNotification.setStyle(new NotificationCompat.BigTextStyle().bigText(expandableContent));
        update();
    }

    public void setSmallIcon(int smallIconResourceId)
    {
        mAppNotification.setSmallIcon(smallIconResourceId);
        update();
    }

    public void setLargeIcon(Bitmap largeIcon)
    {
        mAppNotification.setLargeIcon(largeIcon);
        update();
    }

    public void setProgressBar(int max, int current, boolean indeterminate)
    {
        mAppNotification.setProgress(max, current, indeterminate);
        update();
    }

    public void createNotificationChannel(String channelName)
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationManager Manager = mActivityInstance.getSystemService(NotificationManager.class);
            NotificationChannel Channel;

            Channel = new NotificationChannel(NOTIFICATION_CHANNEL_ID,
                                              channelName,
                                              NotificationManager.IMPORTANCE_DEFAULT
                                              );

            Manager.createNotificationChannel(Channel);
        }
    }

    public void deleteNotificationChannel()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationManager Manager = mActivityInstance.getSystemService(NotificationManager.class);
            Manager.deleteNotificationChannel(NOTIFICATION_CHANNEL_ID);
        }
    }
}
