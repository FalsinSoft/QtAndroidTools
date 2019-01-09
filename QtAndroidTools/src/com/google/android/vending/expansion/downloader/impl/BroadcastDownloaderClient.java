package com.google.android.vending.expansion.downloader.impl;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;

/**
 * This class is used by client to receive download events. The current
 * implementation uses {@link LocalBroadcastManager}, so client should
 * subscribe for the updates within the application context.
 *
 * <br /> <br />
 *
 * <b>Note:</b> You should preferably register the receiver through the
 * methods {@link #register(Context)} and {@link #unregister(Context)}.
 * The broadcast is not sticky, so client should use
 * {@link IDownloaderService#requestDownloadStatus()} to receive the
 * current download status immediately (see {@link DownloaderProxy}).
 *
 *
 * @since 5.0.0
 */
public abstract class BroadcastDownloaderClient extends BroadcastReceiver implements IDownloaderClient {

    public static final String ACTION_STATE_CHANGED = "com.google.android.vending.expansion.downloader.ACTION_STATE_CHANGED";
    public static final String ACTION_PROGRESS = "com.google.android.vending.expansion.downloader.ACTION_PROGRESS";

    public static final String EXTRA_NEW_STATE = "newState";
    public static final String EXTRA_PROGRESS = "progress";

    public void register(Context context) {
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_STATE_CHANGED);
        filter.addAction(ACTION_PROGRESS);
        LocalBroadcastManager.getInstance(context).registerReceiver(this, filter);
    }

    public void unregister(Context context) {
        LocalBroadcastManager.getInstance(context).unregisterReceiver(this);
    }

    @Override public void onReceive(Context context, Intent intent) {
        switch (intent.getAction()) {
            case ACTION_STATE_CHANGED:
                int state = intent.getIntExtra(EXTRA_NEW_STATE, -1);
                onDownloadStateChanged(state);
                break;
            case ACTION_PROGRESS:
                DownloadProgressInfo info = intent.getParcelableExtra(EXTRA_PROGRESS);
                onDownloadProgress(info);
                break;
        }
    }
}
