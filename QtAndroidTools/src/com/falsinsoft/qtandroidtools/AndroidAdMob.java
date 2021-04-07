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

import android.app.Activity;
import android.content.Context;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.ads.mediation.admob.AdMobAdapter;
import android.os.Bundle;

public class AndroidAdMob
{
    private static boolean mMobileAdsInitialized = false;
    private boolean mNonPersonalizedAds = false;

    public AndroidAdMob(Activity activityInstance)
    {
        if(mMobileAdsInitialized == false)
        {
            MobileAds.initialize(activityInstance, new MobileAdsInitializationListener());
        }
    }

    protected boolean isMobileAdsInitialized()
    {
        return mMobileAdsInitialized;
    }

    public void setNonPersonalizedAds(boolean npa)
    {
        mNonPersonalizedAds = npa;
    }

    protected void setExtraOptions(AdRequest.Builder builder)
    {
        if(mNonPersonalizedAds)
        {
            final Bundle extras = new Bundle();
            extras.putString("npa", "1");
            builder.addNetworkExtrasBundle(AdMobAdapter.class, extras);
        }
    }

    private class MobileAdsInitializationListener implements OnInitializationCompleteListener
    {
        @Override
        public void onInitializationComplete(InitializationStatus initializationStatus)
        {
            mMobileAdsInitialized = true;
        }
    }
}
