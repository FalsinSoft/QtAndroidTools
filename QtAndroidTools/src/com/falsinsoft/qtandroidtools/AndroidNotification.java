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
    private final String NOTIFICATION_CHANNEL_ID;
    private final Activity mActivityInstance;
    private final int mNotificationId;

    private int mSmallIconResourceId = 0;
    private Bitmap mLargeIcon = null;

    public AndroidNotification(Activity ActivityInstance, int InstanceId)
    {
        NOTIFICATION_CHANNEL_ID = (ActivityInstance.getClass().getName() + Integer.toString(InstanceId));
        mActivityInstance = ActivityInstance;
        mNotificationId = InstanceId;
    }

    public void show(String title, String content)
    {
        NotificationCompat.Builder AppNotification = new NotificationCompat.Builder(mActivityInstance, NOTIFICATION_CHANNEL_ID);
        NotificationManagerCompat Manager = NotificationManagerCompat.from(mActivityInstance);

        AppNotification.setSmallIcon(mSmallIconResourceId);
        AppNotification.setContentTitle(title);
        AppNotification.setContentText(content);
        AppNotification.setPriority(NotificationCompat.PRIORITY_DEFAULT);
        if(mLargeIcon != null) AppNotification.setLargeIcon(mLargeIcon);

        Manager.notify(mNotificationId, AppNotification.build());
    }

    public void cancel()
    {
        NotificationManagerCompat Manager = NotificationManagerCompat.from(mActivityInstance);
        Manager.cancel(mNotificationId);
    }

    public void setSmallIconResourceId(int iconId)
    {
        mSmallIconResourceId = iconId;
    }

    public void setLargeIcon(Bitmap icon)
    {
        mLargeIcon = icon;
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
