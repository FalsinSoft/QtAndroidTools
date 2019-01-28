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
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdListener;
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

public class AdMobBanner
{
    private final Activity mActivityInstance;
    private final ViewGroup mViewGroup;
    private final BannerListener mBannerListener;

    private AdView mBannerView = null;
    private boolean mBannerLoaded = false;
    private BannerSize mBannerSize = new BannerSize();

    public AdMobBanner(Activity ActivityInstance)
    {
        mViewGroup = (ViewGroup)ActivityInstance.getWindow().getDecorView().getRootView();
        mBannerListener = new BannerListener();
        mActivityInstance = ActivityInstance;
    }

    public BannerSize getSize()
    {
        return mBannerSize;
    }

    public void setType(final int type)
    {
        if(mBannerView == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                AdSize BannerSize = AdSize.BANNER;

                switch(type)
                {
                    case TYPE_BANNER:
                        BannerSize = AdSize.BANNER;
                        break;
                    case TYPE_FULL_BANNER:
                        BannerSize = AdSize.FULL_BANNER;
                        break;
                    case TYPE_LARGE_BANNER:
                        BannerSize = AdSize.LARGE_BANNER;
                        break;
                    case TYPE_MEDIUM_RECTANGLE:
                        BannerSize = AdSize.MEDIUM_RECTANGLE;
                        break;
                    case TYPE_SMART_BANNER:
                        BannerSize = AdSize.SMART_BANNER;
                        break;
                    case TYPE_WIDE_SKYSCRAPER:
                        BannerSize = AdSize.WIDE_SKYSCRAPER;
                        break;
                }
                mBannerView.setAdSize(BannerSize);

                mBannerSize.width  = BannerSize.getWidth();
                mBannerSize.height = BannerSize.getHeight();
            }
        });
        UiThread.exec();
    }

    public void setPos(final BannerPos pos)
    {
        if(mBannerView == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                Rect VisibleFrame = new Rect();

                mActivityInstance.getWindow().getDecorView().getWindowVisibleDisplayFrame(VisibleFrame);

                mBannerView.setX(pos.x);
                mBannerView.setY(VisibleFrame.top + pos.y); // Add the height of the system status bar on top
            }
        });
        UiThread.exec();
    }

    public void setUnitId(final String unitId)
    {
        if(mBannerView == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mBannerView.setAdUnitId(unitId);
            }
        });
        UiThread.exec();
    }

    public void show()
    {
        if(mBannerView == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                if(mBannerLoaded == false)
                {
                    AdRequest.Builder BannerRequest = new AdRequest.Builder();
                    mBannerView.loadAd(BannerRequest.build());
                    bannerEvent(EVENT_LOADING);
                }
                mBannerView.setVisibility(View.VISIBLE);
            }
        });
    }

    public void hide()
    {
        if(mBannerView == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mBannerView.setVisibility(View.GONE);
            }
        });
    }

    public void appStateChanged(int newState)
    {
        switch(newState)
        {
            case APP_STATE_CREATE:
                createBanner();
                break;
            case APP_STATE_START:
                if(mBannerView != null) mBannerView.resume();
                break;
            case APP_STATE_STOP:
                if(mBannerView != null) mBannerView.pause();
                break;
            case APP_STATE_DESTROY:
                destroyBanner();
                break;
        }
    }

    private void createBanner()
    {
        if(mBannerView != null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                final FrameLayout.LayoutParams LayoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);

                mBannerView = new AdView(mActivityInstance);
                mBannerView.setLayoutParams(LayoutParams);
                mBannerView.setBackgroundColor(Color.BLACK);
                mBannerView.setAdListener(mBannerListener);
                mBannerView.setVisibility(View.GONE);
                mViewGroup.addView(mBannerView);
            }
        });
        UiThread.exec();
    }

    private void destroyBanner()
    {
        if(mBannerView == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mViewGroup.removeView(mBannerView);
                mBannerView.destroy();
                mBannerView = null;
                mBannerLoaded = false;
            }
        });
        UiThread.exec();
    }

    private class BannerListener extends AdListener
    {
        public void onAdLoaded()
        {
            bannerEvent(EVENT_LOADED);
            mBannerLoaded = true;
        }

        public void onAdClosed()
        {
            bannerEvent(EVENT_CLOSED);
        }

        public void onAdLeftApplication()
        {
            bannerEvent(EVENT_CLICKED);
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

            bannerError(errorId);
        }
    }

    public static class BannerPos
    {
        public int x = 0;
        public int y = 0;
    }

    public static class BannerSize
    {
        public int width = 0;
        public int height = 0;
    }

    private static final int ERROR_INTERNAL = 0;
    private static final int ERROR_NETWORK = 1;
    private static final int ERROR_INVALID_REQUEST = 2;
    private static final int ERROR_NO_FILL = 3;

    private static final int EVENT_LOADING = 0;
    private static final int EVENT_LOADED = 1;
    private static final int EVENT_CLOSED = 2;
    private static final int EVENT_CLICKED = 3;

    private static final int TYPE_BANNER = 0;
    private static final int TYPE_FULL_BANNER = 1;
    private static final int TYPE_LARGE_BANNER = 2;
    private static final int TYPE_MEDIUM_RECTANGLE = 3;
    private static final int TYPE_SMART_BANNER = 4;
    private static final int TYPE_WIDE_SKYSCRAPER = 5;

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static native void bannerEvent(int eventId);
    private static native void bannerError(int errorId);
}
