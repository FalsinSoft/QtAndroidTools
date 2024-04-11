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
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;
import com.google.android.gms.ads.rewarded.ServerSideVerificationOptions;
import com.google.android.gms.ads.OnUserEarnedRewardListener;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.AdError;
import androidx.annotation.NonNull;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class AndroidAdMobRewardedAd extends AndroidAdMob
{
    private final Activity mActivityInstance;
    private final RewardedLoadCallback mRewardedLoadCallback;
    private final UserEarnedRewardListener mUserEarnedRewardListener;
    private final RewardedFullScreenContentCallback mRewardedFullScreenContentCallback;

    private RewardedAd mRewardedAd = null;
    private String mUnitId = null;
    private String mCustomData = null;

    public AndroidAdMobRewardedAd(Context context)
    {
        super(context);
        mActivityInstance = (Activity)context;
        mRewardedLoadCallback = new RewardedLoadCallback();
        mUserEarnedRewardListener = new UserEarnedRewardListener();
        mRewardedFullScreenContentCallback = new RewardedFullScreenContentCallback();
    }

    public void setUnitId(final String unitId)
    {
        mUnitId = unitId;
    }

    public void setCustomData(final String customData)
    {
        mCustomData = customData;
    }

    public void load()
    {
        if(mUnitId == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                AdRequest.Builder rewardedRequest = new AdRequest.Builder();

                mRewardedAd = null;
                rewardedAdEvent(EVENT_LOADING);
                setExtraOptions(rewardedRequest);
                RewardedAd.load(mActivityInstance, mUnitId, rewardedRequest.build(), mRewardedLoadCallback);
            }
        });
    }

    public void show()
    {
        if(mRewardedAd == null)
        {
            return;
        }

        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mRewardedAd.show(mActivityInstance, mUserEarnedRewardListener);
            }
        });
    }

    private class RewardedLoadCallback extends RewardedAdLoadCallback
    {
        @Override
        public void onAdLoaded(@NonNull RewardedAd rewardedAd)
        {
            if(mCustomData != null)
            {
                ServerSideVerificationOptions.Builder options = new ServerSideVerificationOptions.Builder();
                options.setCustomData(mCustomData);
                rewardedAd.setServerSideVerificationOptions(options.build());
            }
            rewardedAd.setFullScreenContentCallback(mRewardedFullScreenContentCallback);
            mRewardedAd = rewardedAd;
            rewardedAdEvent(EVENT_LOADED);
        }

        @Override
        public void onAdFailedToLoad(@NonNull LoadAdError loadAdError)
        {
            mRewardedAd = null;
            rewardedAdEvent(EVENT_LOAD_ERROR);
        }
    }

    private class RewardedFullScreenContentCallback extends FullScreenContentCallback
    {
        @Override
        public void onAdClicked()
        {
            rewardedAdEvent(EVENT_CLICKED);
        }

        @Override
        public void onAdDismissedFullScreenContent()
        {
            rewardedAdEvent(EVENT_DISMISSED);
            mRewardedAd = null;
        }

        @Override
        public void onAdFailedToShowFullScreenContent(AdError adError)
        {
            rewardedAdEvent(EVENT_SHOW_FAILED);
            mRewardedAd = null;
        }

        @Override
        public void onAdImpression()
        {
            rewardedAdEvent(EVENT_IMPRESSION);
        }

        @Override
        public void onAdShowedFullScreenContent()
        {
            rewardedAdEvent(EVENT_SHOWED);
        }
    }

    private class UserEarnedRewardListener implements OnUserEarnedRewardListener
    {
        @Override
        public void onUserEarnedReward(@NonNull RewardItem rewardItem)
        {
            rewardedAdReward(rewardItem.getType(), rewardItem.getAmount());
        }
    }

    private static final int EVENT_LOAD_ERROR = 0;
    private static final int EVENT_LOADING = 1;
    private static final int EVENT_LOADED = 2;
    private static final int EVENT_CLICKED = 3;
    private static final int EVENT_DISMISSED = 4;
    private static final int EVENT_SHOW_FAILED = 5;
    private static final int EVENT_IMPRESSION = 6;
    private static final int EVENT_SHOWED = 7;

    private static native void rewardedAdReward(String type, int amount);
    private static native void rewardedAdEvent(int eventId);
}
