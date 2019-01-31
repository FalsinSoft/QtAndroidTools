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

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.InterstitialAd;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.ViewGroup;
import android.util.Log;
import android.graphics.Rect;
import android.widget.FrameLayout;
import android.graphics.Color;

public class AdMobInterstitial
{
    private final Activity mActivityInstance;
    private final InterstitialListener mInterstitialListener;

    private InterstitialAd mInterstitialAd = null;

    public AdMobInterstitial(Activity ActivityInstance)
    {
        mInterstitialListener = new InterstitialListener();
        mActivityInstance = ActivityInstance;
    }

    public void setUnitId(final String unitId)
    {
        if(mInterstitialAd == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mInterstitialAd.setAdUnitId(unitId);
            }
        });
        UiThread.exec();
    }

    public void load()
    {
        if(mInterstitialAd == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                AdRequest.Builder InterstitialRequest = new AdRequest.Builder();
                mInterstitialAd.loadAd(InterstitialRequest.build());
                interstitialEvent(EVENT_LOADING);
            }
        });
    }

    public void show()
    {
        if(mInterstitialAd == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mInterstitialAd.show();
            }
        });
    }

    public void appStateChanged(int newState)
    {
        switch(newState)
        {
            case APP_STATE_CREATE:
                createInterstitial();
                break;
            case APP_STATE_START:
            case APP_STATE_STOP:
                break;
            case APP_STATE_DESTROY:
                destroyInterstitial();
                break;
        }
    }

    private void createInterstitial()
    {
        if(mInterstitialAd != null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mInterstitialAd = new InterstitialAd(mActivityInstance);
                mInterstitialAd.setAdListener(mInterstitialListener);
            }
        });
        UiThread.exec();
    }

    private void destroyInterstitial()
    {
        if(mInterstitialAd == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mInterstitialAd = null;
            }
        });
        UiThread.exec();
    }

    private class InterstitialListener extends AdListener
    {
        public void onAdLoaded()
        {
            interstitialEvent(EVENT_LOADED);
        }

        public void onAdClosed()
        {
            interstitialEvent(EVENT_CLOSED);
        }

        public void onAdLeftApplication()
        {
            interstitialEvent(EVENT_CLICKED);
        }

        public void onAdFailedToLoad(int errorCode)
        {
            int errorId = 0;

            switch(errorCode)
            {
                case AdRequest.ERROR_CODE_INTERNAL_ERROR:
                    errorId = ERROR_INTERNAL;
                    break;
                case AdRequest.ERROR_CODE_NETWORK_ERROR:
                    errorId = ERROR_NETWORK;
                    break;
                case AdRequest.ERROR_CODE_INVALID_REQUEST:
                    errorId = ERROR_INVALID_REQUEST;
                    break;
                case AdRequest.ERROR_CODE_NO_FILL:
                    errorId = ERROR_NO_FILL;
                    break;
            }

            interstitialError(errorId);
        }
    }

    private static final int ERROR_INTERNAL = 0;
    private static final int ERROR_NETWORK = 1;
    private static final int ERROR_INVALID_REQUEST = 2;
    private static final int ERROR_NO_FILL = 3;

    private static final int EVENT_LOADING = 0;
    private static final int EVENT_LOADED = 1;
    private static final int EVENT_CLOSED = 2;
    private static final int EVENT_CLICKED = 3;

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static native void interstitialEvent(int eventId);
    private static native void interstitialError(int errorId);
}
