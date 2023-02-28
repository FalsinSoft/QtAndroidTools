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
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.AdError;
import androidx.annotation.NonNull;
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

public class AndroidAdMobInterstitial extends AndroidAdMob
{
    private final Activity mActivityInstance;
    private final InterstitialLoadCallback mInterstitialLoadCallback;
    private final InterstitialFullScreenContentCallback mInterstitialFullScreenContentCallback;

    private InterstitialAd mInterstitialAd = null;
    private String mUnitId = null;

    public AndroidAdMobInterstitial(Activity activityInstance)
    {
        super(activityInstance);
        mActivityInstance = activityInstance;
        mInterstitialLoadCallback = new InterstitialLoadCallback();
        mInterstitialFullScreenContentCallback = new InterstitialFullScreenContentCallback();
    }

    public void setUnitId(final String unitId)
    {
        mUnitId = unitId;
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
                AdRequest.Builder interstitialRequest = new AdRequest.Builder();

                mInterstitialAd = null;
                interstitialEvent(EVENT_LOADING);
                setExtraOptions(interstitialRequest);
                InterstitialAd.load(mActivityInstance, mUnitId, interstitialRequest.build(), mInterstitialLoadCallback);
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
                mInterstitialAd.show(mActivityInstance);
            }
        });
    }

    private class InterstitialLoadCallback extends InterstitialAdLoadCallback
    {
        @Override
        public void onAdLoaded(@NonNull InterstitialAd interstitialAd)
        {
            interstitialAd.setFullScreenContentCallback(mInterstitialFullScreenContentCallback);
            mInterstitialAd = interstitialAd;
            interstitialEvent(EVENT_LOADED);
        }

        @Override
        public void onAdFailedToLoad(@NonNull LoadAdError loadAdError)
        {
            mInterstitialAd = null;
            interstitialEvent(EVENT_LOAD_ERROR);
        }
    }

    private class InterstitialFullScreenContentCallback extends FullScreenContentCallback
    {
        @Override
        public void onAdClicked()
        {
            interstitialEvent(EVENT_CLICKED);
        }

        @Override
        public void onAdDismissedFullScreenContent()
        {
            interstitialEvent(EVENT_DISMISSED);
            mInterstitialAd = null;
        }

        @Override
        public void onAdFailedToShowFullScreenContent(AdError adError)
        {
            interstitialEvent(EVENT_SHOW_FAILED);
            mInterstitialAd = null;
        }

        @Override
        public void onAdImpression()
        {
            interstitialEvent(EVENT_IMPRESSION);
        }

        @Override
        public void onAdShowedFullScreenContent()
        {
            interstitialEvent(EVENT_SHOWED);
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

    private static native void interstitialEvent(int eventId);
}
