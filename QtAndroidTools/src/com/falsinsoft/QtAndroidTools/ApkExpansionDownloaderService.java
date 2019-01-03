package com.falsinsoft.QtAndroidTools;

import com.google.android.vending.expansion.downloader.impl.DownloaderService;

public class ApkExpansionDownloaderService extends DownloaderService 
{
    public static String BASE64_PUBLIC_KEY = null;
    public static byte[] SALT = null;

    @Override
    public String getPublicKey() 
{
        return BASE64_PUBLIC_KEY;
    }

    @Override
    public byte[] getSALT() 
    {
        return SALT;
    }

    @Override
    public String getAlarmReceiverClassName() 
    {
        return ApkExpansionDownloaderAlarmReceiver.class.getName();
    }

}
