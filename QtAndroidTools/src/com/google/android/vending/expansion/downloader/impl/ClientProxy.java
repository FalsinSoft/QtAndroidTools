package com.google.android.vending.expansion.downloader.impl;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.IDownloaderClient;

/**
 * Internal class used for communication with client. In current
 * implementation the communication is done via {@link LocalBroadcastManager}.
 * Commands are received by the {@link BroadcastDownloaderClient}.
 */
class ClientProxy implements IDownloaderClient {

    private LocalBroadcastManager mBroadcastManager;

    ClientProxy(Context ctx) {
        mBroadcastManager = LocalBroadcastManager.getInstance(ctx);
    }

    @Override
    public void onDownloadStateChanged(int newState) {
        Bundle params = new Bundle(1);
        params.putInt(BroadcastDownloaderClient.EXTRA_NEW_STATE, newState);
        send(BroadcastDownloaderClient.ACTION_STATE_CHANGED, params);
    }

    @Override
    public void onDownloadProgress(DownloadProgressInfo progress) {
        Bundle params = new Bundle(1);
        params.putParcelable(BroadcastDownloaderClient.EXTRA_PROGRESS, progress);
        send(BroadcastDownloaderClient.ACTION_PROGRESS, params);
    }

    private void send(String action, Bundle params) {
        Intent intent = new Intent(action);
        intent.putExtras(params);
        mBroadcastManager.sendBroadcast(intent);
    }
}
