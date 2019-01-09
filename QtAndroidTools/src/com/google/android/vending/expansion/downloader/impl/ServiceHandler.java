package com.google.android.vending.expansion.downloader.impl;

import android.os.Handler;
import android.os.Message;
import com.google.android.vending.expansion.downloader.IDownloaderService;

class ServiceHandler extends Handler {

    public static final String PARAMS_FLAGS = "flags";

    public static final int MSG_REQUEST_ABORT_DOWNLOAD = 1;
    public static final int MSG_REQUEST_PAUSE_DOWNLOAD = 2;
    public static final int MSG_SET_DOWNLOAD_FLAGS = 3;
    public static final int MSG_REQUEST_CONTINUE_DOWNLOAD = 4;
    public static final int MSG_REQUEST_DOWNLOAD_STATE = 5;

    private final IDownloaderService service;

    ServiceHandler(IDownloaderService service) {
        this.service = service;
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_REQUEST_ABORT_DOWNLOAD:
                service.requestAbortDownload();
                break;
            case MSG_REQUEST_CONTINUE_DOWNLOAD:
                service.requestContinueDownload();
                break;
            case MSG_REQUEST_PAUSE_DOWNLOAD:
                service.requestPauseDownload();
                break;
            case MSG_SET_DOWNLOAD_FLAGS:
                service.setDownloadFlags(msg.getData().getInt(PARAMS_FLAGS));
                break;
            case MSG_REQUEST_DOWNLOAD_STATE:
                service.requestDownloadStatus();
                break;
        }
    }

}
