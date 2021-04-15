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

import com.google.android.vending.expansion.downloader.*;
import com.google.android.vending.expansion.downloader.impl.*;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.app.Activity;
import android.os.Messenger;
import android.app.PendingIntent;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.os.Build;
import android.app.NotificationChannel;
import android.app.NotificationManager;

public class AndroidApkExpansionFiles
{
    private static final String TAG = "AndroidApkExpansionFiles";

    private final String NOTIFICATION_CHANNEL_ID;
    private final DownloaderClient mDownloaderClient;
    private final DownloaderProxy mDownloaderProxy;
    private final Activity mActivityInstance;
    private static String[] mStringsList = null;

    public AndroidApkExpansionFiles(Activity activityInstance)
    {
        NOTIFICATION_CHANNEL_ID = activityInstance.getClass().getName();
        mDownloaderClient = new DownloaderClient();
        mDownloaderProxy = new DownloaderProxy(activityInstance);
        mActivityInstance = activityInstance;
    }

    public void setStringsList(String[] stringsList)
    {
        mStringsList = stringsList;
    }

    public boolean isAPKFileDelivered(boolean isMain, int fileVersion, int fileSize)
    {
        final String fileName = Helpers.getExpansionAPKFileName(mActivityInstance, isMain, fileVersion);
        return Helpers.doesFileExist(mActivityInstance, fileName, fileSize, false);
    }

    public String getExpansionAPKFileName(boolean isMain, int fileVersion)
    {
        final String fileName = Helpers.getExpansionAPKFileName(mActivityInstance, isMain, fileVersion);
        return Helpers.generateSaveFileName(mActivityInstance, fileName);
    }

    public void sendRequest(int requestID)
    {
        mDownloaderProxy.connect();

        switch(requestID)
        {
            case REQUEST_ABORT_DOWNLOAD:
                mDownloaderProxy.requestAbortDownload();
                break;
            case REQUEST_PAUSE_DOWNLOAD:
                mDownloaderProxy.requestPauseDownload();
                break;
            case REQUEST_CONTINUE_DOWNLOAD:
                mDownloaderProxy.requestContinueDownload();
                break;
            case REQUEST_DOWNLOAD_STATUS:
                mDownloaderProxy.requestDownloadStatus();
                break;
        }

        mDownloaderProxy.disconnect();
    }

    public void appStateChanged(int newState)
    {
        switch(newState)
        {
            case APP_STATE_CREATE:
                if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                {
                    NotificationManager manager = mActivityInstance.getSystemService(NotificationManager.class);
                    NotificationChannel channel;

                    channel = new NotificationChannel(NOTIFICATION_CHANNEL_ID,
                                                      getString(STRING_NOTIFICATION_CHANNEL_NAME),
                                                      NotificationManager.IMPORTANCE_DEFAULT
                                                      );

                    manager.createNotificationChannel(channel);
                }
                mDownloaderClient.register(mActivityInstance);
                break;
            case APP_STATE_START:
                mDownloaderClient.register(mActivityInstance);
                break;
            case APP_STATE_STOP:
                mDownloaderClient.unregister(mActivityInstance);
                break;
            case APP_STATE_DESTROY:
            /*
                if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                {
                    NotificationManager manager = mActivityInstance.getSystemService(NotificationManager.class);
                    manager.deleteNotificationChannel(NOTIFICATION_CHANNEL_ID);
                }
            */
                mDownloaderClient.unregister(mActivityInstance);
                break;
        }
    }

    public int startDownloader(String BASE64_PUBLIC_KEY, byte[] SALT)
    {
        int downloadResult = -1;

        try
        {
            Intent intentToLaunchThisActivityFromNotification;
            PendingIntent pendingActivity;

            intentToLaunchThisActivityFromNotification = new Intent(mActivityInstance, mActivityInstance.getClass());
            intentToLaunchThisActivityFromNotification.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);

            pendingActivity = PendingIntent.getActivity(mActivityInstance,
                                                        0,
                                                        intentToLaunchThisActivityFromNotification,
                                                        PendingIntent.FLAG_UPDATE_CURRENT
                                                        );

            downloadResult = DownloaderService.startDownloadServiceIfRequired(mActivityInstance,
                                                                              NOTIFICATION_CHANNEL_ID,
                                                                              pendingActivity,
                                                                              SALT,
                                                                              BASE64_PUBLIC_KEY
                                                                              );
        }
        catch(NameNotFoundException e)
        {
            e.printStackTrace();
        }

        return downloadResult;
    }

    public static String getString(int stringID)
    {
        String text = "";

        if(mStringsList != null && stringID >= 0 && stringID < mStringsList.length)
        {
            text = mStringsList[stringID];
        }

        return text;
    }

    private class DownloaderClient extends BroadcastDownloaderClient
    {
        @Override
        public void onDownloadStateChanged(int newState)
        {
            downloadStateChanged(newState);
        }

        @Override
        public void onDownloadProgress(DownloadProgressInfo progress)
        {
            downloadProgress(progress.mOverallTotal, progress.mOverallProgress, progress.mTimeRemaining, progress.mCurrentSpeed);
        }
    }
	
    public static final int STRING_IDLE = 0;
    public static final int STRING_FETCHING_URL = 1;
    public static final int STRING_CONNECTING = 2;
    public static final int STRING_DOWNLOADING = 3;
    public static final int STRING_COMPLETED = 4;
    public static final int STRING_PAUSED_NETWORK_UNAVAILABLE = 5;
    public static final int STRING_PAUSED_BY_REQUEST = 6;
    public static final int STRING_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION = 7;
    public static final int STRING_PAUSED_NEED_CELLULAR_PERMISSION = 8;
    public static final int STRING_PAUSED_WIFI_DISABLED = 9;
    public static final int STRING_PAUSED_NEED_WIFI = 10;
    public static final int STRING_PAUSED_ROAMING = 11;
    public static final int STRING_PAUSED_NETWORK_SETUP_FAILURE = 12;
    public static final int STRING_PAUSED_SDCARD_UNAVAILABLE = 13;
    public static final int STRING_FAILED_UNLICENSED = 14;
    public static final int STRING_FAILED_FETCHING_URL = 15;
    public static final int STRING_FAILED_SDCARD_FULL = 16;
    public static final int STRING_FAILED_CANCELED = 17;
    public static final int STRING_FAILED = 18;
    public static final int STRING_UNKNOWN = 19;
    public static final int STRING_TIME_LEFT = 20;
    public static final int STRING_NOTIFICATION_CHANNEL_NAME = 21;

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static final int REQUEST_ABORT_DOWNLOAD = 0;
    private static final int REQUEST_PAUSE_DOWNLOAD = 1;
    private static final int REQUEST_CONTINUE_DOWNLOAD = 2;
    private static final int REQUEST_DOWNLOAD_STATUS = 3;

    private static native void downloadStateChanged(int newState);
    private static native void downloadProgress(long overallTotal, long overallProgress, long timeRemaining, float currentSpeed);
}

