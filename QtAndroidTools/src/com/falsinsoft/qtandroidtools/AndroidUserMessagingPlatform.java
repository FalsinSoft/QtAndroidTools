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

import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import androidx.annotation.Nullable;
import com.google.android.ump.ConsentForm;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentInformation.PrivacyOptionsRequirementStatus;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

public class AndroidUserMessagingPlatform
{
    private final ConsentInformation mConsentInformation;
    private final ConsentListener mConsentListener;
    private final Activity mActivityInstance;

    public AndroidUserMessagingPlatform(Activity activityInstance)
    {
        mConsentInformation = UserMessagingPlatform.getConsentInformation(activityInstance);
        mConsentListener = new ConsentListener();
        mActivityInstance = activityInstance;
    }

    public void resetConsentInformation()
    {
        mConsentInformation.reset();
    }

    public boolean canRequestAds()
    {
        return mConsentInformation.canRequestAds();
    }

    public void loadAndShowConsentFormIfRequired(boolean underAgeOfConsent)
    {
        final ConsentRequestParameters params = new ConsentRequestParameters.Builder()
                                                                            .setTagForUnderAgeOfConsent(underAgeOfConsent)
                                                                            .build();

        mConsentInformation.requestConsentInfoUpdate(mActivityInstance, params, mConsentListener, mConsentListener);
    }

    public void showPrivacyOptionsForm()
    {
        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                UserMessagingPlatform.showPrivacyOptionsForm(mActivityInstance, mConsentListener);
            }
        });
    }

    private class ConsentListener implements ConsentInformation.OnConsentInfoUpdateSuccessListener,
                                             ConsentInformation.OnConsentInfoUpdateFailureListener,
                                             ConsentForm.OnConsentFormDismissedListener
    {
        @Override
        public void onConsentInfoUpdateSuccess()
        {
            mActivityInstance.runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    UserMessagingPlatform.loadAndShowConsentFormIfRequired(mActivityInstance, mConsentListener);
                }
            });
        }

        @Override
        public void onConsentInfoUpdateFailure(FormError formError)
        {
            consentFormRequestFailure(formError.getMessage());
        }

        @Override
        public void onConsentFormDismissed(@Nullable FormError formError)
        {
            final boolean privacyOptionsRequired = (mConsentInformation.getPrivacyOptionsRequirementStatus() == PrivacyOptionsRequirementStatus.REQUIRED) ? true : false;
            final boolean consentGathered = (formError == null) ? true : false;

            consentFormDismissed(consentGathered, privacyOptionsRequired);
        }
    }

    private static native void consentFormRequestFailure(String errorMessage);
    private static native void consentFormDismissed(boolean consentGathered, boolean privacyOptionsRequired);
}
