package com.google.android.vending.expansion.downloader.impl;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.*;
import android.util.Log;
import com.google.android.vending.expansion.downloader.Constants;
import com.google.android.vending.expansion.downloader.IDownloaderService;

import java.util.LinkedList;
import java.util.Queue;

import static com.google.android.vending.expansion.downloader.impl
        .ServiceHandler.*;

/**
 * This class is used by the client to issue commands to the {@link
 * DownloaderService} (such as to pause and resume downloads).
 * <p>
 * Most importantly, you must first call {@link #connect()} method to
 * establish the connection with the service. All the calls to {@link
 * IDownloaderService} that happen when connection is being
 * established will be queued and delivered when connection is ready.
 */
public final class DownloaderProxy implements IDownloaderService,
        ServiceConnection {

    private final Context mContext;
    private Messenger mMessenger;
    private boolean mConnected;
    private boolean connectCalled;
    private final Queue<Message> mMessages = new LinkedList<>();

    public DownloaderProxy(Context context) {
        this.mContext = context;
    }

    @Override
    public final void onServiceConnected(ComponentName name, IBinder service) {
        mMessenger = new Messenger(service);
        mConnected = true;
        drainMessages();
    }

    @Override
    public final void onServiceDisconnected(ComponentName name) {
        mMessenger = null;
        mConnected = false;
    }

    @Override
    public void requestAbortDownload() {
        send(MSG_REQUEST_ABORT_DOWNLOAD, new Bundle());
    }

    @Override
    public void requestPauseDownload() {
        send(MSG_REQUEST_PAUSE_DOWNLOAD, new Bundle());
    }

    @Override
    public void setDownloadFlags(int flags) {
        Bundle params = new Bundle();
        params.putInt(PARAMS_FLAGS, flags);
        send(MSG_SET_DOWNLOAD_FLAGS, params);
    }

    @Override
    public void requestContinueDownload() {
        send(MSG_REQUEST_CONTINUE_DOWNLOAD, new Bundle());
    }

    @Override
    public void requestDownloadStatus() {
        send(MSG_REQUEST_DOWNLOAD_STATE, new Bundle());
    }

    private void drainMessages() {
        while (mMessenger != null && !mMessages.isEmpty()) {
            try {
                mMessenger.send(mMessages.peek());
                mMessages.remove();
            } catch (RemoteException e) {
                Log.e(Constants.TAG, "send: ", e);
            }
        }
    }

    private void send(int method, Bundle params) {

        if (!connectCalled) {
            throw new IllegalStateException("connect() method was not called");
        }

        Message m = Message.obtain(null, method);
        m.setData(params);

        mMessages.add(m);

        if (mConnected) {
            drainMessages();
        }
    }

    public void connect() {
        Intent bindIntent = new Intent(mContext.getApplicationContext(),
                DownloaderService.class);
        if (!mContext.getApplicationContext().bindService(bindIntent, this,
                Context.BIND_DEBUG_UNBIND)) {
            Log.w(Constants.TAG, "Service not bound. Check Manifest.xml " +
                    "declaration");
        } else {
            connectCalled = true;
        }
    }

    public void disconnect() {
        if (mConnected) {
            mContext.getApplicationContext().unbindService(this);
            connectCalled = false;
        }
    }
}
