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
import android.content.Intent;
import android.app.PendingIntent;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import androidx.core.app.NotificationManagerCompat;
import androidx.core.app.NotificationCompat;

public class AndroidNotification
{
    private NotificationCompat.Builder mAppNotification;
    private final String NOTIFICATION_CHANNEL_ID;
    private final Activity mActivityInstance;
    private final int mNotificationId;

    public AndroidNotification(Activity activityInstance, int instanceId)
    {
        NOTIFICATION_CHANNEL_ID = (activityInstance.getClass().getName() + Integer.toString(instanceId));
        mAppNotification = new NotificationCompat.Builder(activityInstance, NOTIFICATION_CHANNEL_ID);
        mActivityInstance = activityInstance;
        mNotificationId = (instanceId + 1);
        configure();
    }

    private void configure()
    {
        final PendingIntent notificationPendingIntent = PendingIntent.getActivity(mActivityInstance,
                                                                                  mNotificationId,
                                                                                  new Intent(mActivityInstance, mActivityInstance.getClass()),
                                                                                  PendingIntent.FLAG_UPDATE_CURRENT
                                                                                  );
        mAppNotification.setContentIntent(notificationPendingIntent);
        mAppNotification.setOnlyAlertOnce(true);
        mAppNotification.setAutoCancel(true);
    }

    public void show()
    {
        NotificationManagerCompat manager = NotificationManagerCompat.from(mActivityInstance);
        mAppNotification.setPriority(NotificationCompat.PRIORITY_DEFAULT);
        manager.notify(mNotificationId, mAppNotification.build());
    }

    public void cancel()
    {
        NotificationManagerCompat manager = NotificationManagerCompat.from(mActivityInstance);
        manager.cancel(mNotificationId);
    }

    public void setTitle(String title)
    {
        mAppNotification.setContentTitle(title);
    }

    public void setContent(String content)
    {
        mAppNotification.setContentText(content);
    }

    public void setExpandableContent(String expandableContent)
    {
        mAppNotification.setStyle(new NotificationCompat.BigTextStyle().bigText(expandableContent));
    }

    public void setSmallIcon(int smallIconResourceId)
    {
        mAppNotification.setSmallIcon(smallIconResourceId);
    }

    public void setLargeIcon(Bitmap largeIcon)
    {
        mAppNotification.setLargeIcon(largeIcon);
    }

    public void setProgressBar(int max, int current, boolean indeterminate)
    {
        mAppNotification.setProgress(max, current, indeterminate);
    }

    public void createNotificationChannel(String channelName)
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationManager manager = mActivityInstance.getSystemService(NotificationManager.class);
            NotificationChannel channel;

            channel = new NotificationChannel(NOTIFICATION_CHANNEL_ID,
                                              channelName,
                                              NotificationManager.IMPORTANCE_DEFAULT
                                              );

            manager.createNotificationChannel(channel);
        }
    }

    public void deleteNotificationChannel()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationManager manager = mActivityInstance.getSystemService(NotificationManager.class);
            manager.deleteNotificationChannel(NOTIFICATION_CHANNEL_ID);
        }
    }
}
