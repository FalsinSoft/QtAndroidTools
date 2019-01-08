package com.falsinsoft.QtAndroidTools;

import com.google.android.vending.expansion.downloader.Constants;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import com.google.android.vending.expansion.downloader.impl.DownloaderService;

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

import java.util.Arrays;

public class ApkExpansionDownloader implements IDownloaderClient
{
    private final Activity m_ActivityInstance;
    private IDownloaderService m_RemoteService;
    private IStub m_DownloaderClientStub;

    public ApkExpansionDownloader(Activity ActivityInstance)
    {
        final IDownloaderClient Client = this;
        ActivityInstance.runOnUiThread(new Runnable()
        {
            public void run()
            {
                m_DownloaderClientStub = DownloaderClientMarshaller.CreateStub(Client, ApkExpansionDownloaderService.class);
            }
        });
        m_ActivityInstance = ActivityInstance;
    }

    public boolean isAPKFileDelivered(boolean IsMain, int FileVersion, int FileSize)
    {
        final String FileName = Helpers.getExpansionAPKFileName(m_ActivityInstance, IsMain, FileVersion);
        return Helpers.doesFileExist(m_ActivityInstance, FileName, FileSize, false);
    }

    public String getExpansionAPKFileName(boolean IsMain, int FileVersion)
    {
        final String FileName = Helpers.getExpansionAPKFileName(m_ActivityInstance, IsMain, FileVersion);
        return Helpers.generateSaveFileName(m_ActivityInstance, FileName);
    }

    public void enableClientStubConnection(boolean ConnectionEnabled)
    {
        if(ConnectionEnabled == true)
            m_DownloaderClientStub.connect(m_ActivityInstance);
        else
            m_DownloaderClientStub.disconnect(m_ActivityInstance);
    }

    public int startDownloader(String BASE64_PUBLIC_KEY, byte[] SALT)
    {
        int DownloadResult = -1;

        ApkExpansionDownloaderService.BASE64_PUBLIC_KEY = BASE64_PUBLIC_KEY;
        ApkExpansionDownloaderService.SALT = SALT;

        try
        {
            Intent LaunchIntent, IntentToLaunchThisActivityFromNotification;
            PendingIntent PendingActivity;

            LaunchIntent = m_ActivityInstance.getIntent();

            IntentToLaunchThisActivityFromNotification = new Intent(m_ActivityInstance, m_ActivityInstance.getClass());
            IntentToLaunchThisActivityFromNotification.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
            IntentToLaunchThisActivityFromNotification.setAction(LaunchIntent.getAction());


            if(LaunchIntent.getCategories() != null)
            {
                for(String Category : LaunchIntent.getCategories())
                {
                    IntentToLaunchThisActivityFromNotification.addCategory(Category);
                }
            }

            PendingActivity = PendingIntent.getActivity(m_ActivityInstance,
                                                        0,
                                                        IntentToLaunchThisActivityFromNotification,
                                                        PendingIntent.FLAG_UPDATE_CURRENT
                                                        );

            DownloadResult = DownloaderClientMarshaller.startDownloadServiceIfRequired(m_ActivityInstance, PendingActivity, ApkExpansionDownloaderService.class);

            if(DownloadResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED)
            {
                enableClientStubConnection(true);
            }
        }
        catch(NameNotFoundException e)
        {
            e.printStackTrace();
        }

        return DownloadResult;
    }

    @Override
    public void onServiceConnected(Messenger m)
    {
        m_RemoteService = DownloaderServiceMarshaller.CreateProxy(m);
        m_RemoteService.onClientUpdated(m_DownloaderClientStub.getMessenger());
    }

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

    private static native void downloadStateChanged(int newState);
    private static native void downloadProgress(long overallTotal, long overallProgress, long timeRemaining, float currentSpeed);
}

