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
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class AndroidAdMobRewardedVideo
{
    private final Activity mActivityInstance;
    private final RewardedVideoListener mRewardedVideoListener;

    private RewardedVideoAd mRewardedVideoAd = null;

    public AndroidAdMobRewardedVideo(Activity ActivityInstance)
    {
        mRewardedVideoListener = new RewardedVideoListener();
        MobileAds.initialize(ActivityInstance);
        mActivityInstance = ActivityInstance;
    }

    public void load(String unitId)
    {
        final String finalUnitId = unitId;

        if(mRewardedVideoAd == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mRewardedVideoAd.loadAd(finalUnitId, new AdRequest.Builder().build());
                rewardedVideoEvent(EVENT_LOADING);
            }
        });
    }

    public void show()
    {
        if(mRewardedVideoAd == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                if(mRewardedVideoAd.isLoaded() == true)
                {
                    mRewardedVideoAd.show();
                }
            }
        });
    }

    public void appStateChanged(int newState)
    {
        switch(newState)
        {
            case APP_STATE_CREATE:
                createRewardedVideoAd();
                break;
            case APP_STATE_START:
            case APP_STATE_STOP:
                break;
            case APP_STATE_DESTROY:
                destroyRewardedVideoAd();
                break;
        }
    }

    private void createRewardedVideoAd()
    {
        if(mRewardedVideoAd != null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mRewardedVideoAd = MobileAds.getRewardedVideoAdInstance(mActivityInstance);
                mRewardedVideoAd.setRewardedVideoAdListener(mRewardedVideoListener);
            }
        });
        UiThread.exec();
    }

    private void destroyRewardedVideoAd()
    {
        if(mRewardedVideoAd == null)
        {
            return;
        }

        SyncRunOnUiThread UiThread = new SyncRunOnUiThread(mActivityInstance, new SyncRunOnUiThread.SyncRunOnUiThreadListener()
        {
            public void runOnUIThread()
            {
                mRewardedVideoAd.destroy(mActivityInstance);
                mRewardedVideoAd = null;
            }
        });
        UiThread.exec();
    }

    private class RewardedVideoListener implements RewardedVideoAdListener
    {
        @Override
        public void onRewarded(RewardItem reward)
        {
            rewardedVideoReward(reward.getType(), reward.getAmount());
        }

        @Override
        public void onRewardedVideoAdLeftApplication()
        {
            rewardedVideoEvent(EVENT_LEFT_APPLICATION);
        }

        @Override
        public void onRewardedVideoAdClosed()
        {
            rewardedVideoEvent(EVENT_CLOSED);
        }

        @Override
        public void onRewardedVideoAdLoaded()
        {
            rewardedVideoEvent(EVENT_LOADED);
        }

        @Override
        public void onRewardedVideoAdOpened()
        {
            rewardedVideoEvent(EVENT_OPENED);
        }

        @Override
        public void onRewardedVideoStarted()
        {
            rewardedVideoEvent(EVENT_STARTED);
        }

        @Override
        public void onRewardedVideoCompleted()
        {
            rewardedVideoEvent(EVENT_COMPLETED);
        }

        @Override
        public void onRewardedVideoAdFailedToLoad(int errorCode)
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

            rewardedVideoError(errorId);
        }
    }

    private static final int ERROR_INTERNAL = 0;
    private static final int ERROR_NETWORK = 1;
    private static final int ERROR_INVALID_REQUEST = 2;
    private static final int ERROR_NO_FILL = 3;

    private static final int EVENT_LOADING = 0;
    private static final int EVENT_LOADED = 1;
    private static final int EVENT_OPENED = 2;
    private static final int EVENT_CLOSED = 3;
    private static final int EVENT_STARTED = 4;
    private static final int EVENT_COMPLETED = 5;
    private static final int EVENT_LEFT_APPLICATION = 6;

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static native void rewardedVideoReward(String type, int amount);
    private static native void rewardedVideoEvent(int eventId);
    private static native void rewardedVideoError(int errorId);
}
