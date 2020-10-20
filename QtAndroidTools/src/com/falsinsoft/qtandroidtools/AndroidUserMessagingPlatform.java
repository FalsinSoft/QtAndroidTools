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
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

public class AndroidUserMessagingPlatform
{
    private final ConsentInformation mConsentInformation;
    private final ConsentListener mConsentListener;
    private final Activity mActivityInstance;
    private ConsentForm mConsentForm = null;

    public AndroidUserMessagingPlatform(Activity activityInstance)
    {
        mConsentInformation = UserMessagingPlatform.getConsentInformation(activityInstance);
        mConsentListener = new ConsentListener();
        mActivityInstance = activityInstance;
    }

    public void showConsentFormIfRequired()
    {
        requestConsentInfoUpdate();
    }

    public void resetConsentInformation()
    {
        mConsentInformation.reset();
    }

    private void requestConsentInfoUpdate()
    {
        final ConsentRequestParameters params = new ConsentRequestParameters.Builder().build();
        mConsentInformation.requestConsentInfoUpdate(mActivityInstance, params, mConsentListener, mConsentListener);
    }

    private void loadConsentForm()
    {
        if(mConsentInformation.isConsentFormAvailable())
        {
            mActivityInstance.runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    UserMessagingPlatform.loadConsentForm(mActivityInstance, mConsentListener, mConsentListener);
                }
            });
        }
        else
        {
            consentFormShowResult(CONSENT_FORM_NOT_AVAILABLE);
        }
    }

    private void showConsentForm()
    {
        if(mConsentForm != null)
        {
            mActivityInstance.runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    mConsentForm.show(mActivityInstance, mConsentListener);
                }
            });
        }
        else
        {
            consentFormShowResult(CONSENT_FORM_NOT_AVAILABLE);
        }
    }

    private class ConsentListener implements ConsentInformation.OnConsentInfoUpdateSuccessListener,
                                             ConsentInformation.OnConsentInfoUpdateFailureListener,
                                             UserMessagingPlatform.OnConsentFormLoadSuccessListener,
                                             UserMessagingPlatform.OnConsentFormLoadFailureListener,
                                             ConsentForm.OnConsentFormDismissedListener
    {
        @Override
        public void onConsentInfoUpdateSuccess()
        {
            if(mConsentInformation.isConsentFormAvailable())
            {
                loadConsentForm();
            }
            else
            {
                consentFormShowResult(CONSENT_FORM_NOT_AVAILABLE);
            }
        }

        @Override
        public void onConsentInfoUpdateFailure(FormError formError)
        {
            consentFormShowResult(CONSENT_FORM_INFO_UPDATE_FAILURE);
        }

        @Override
        public void onConsentFormLoadSuccess(ConsentForm consentForm)
        {
            final int consentStatus = mConsentInformation.getConsentStatus();

            mConsentForm = consentForm;
            if(consentStatus == ConsentInformation.ConsentStatus.REQUIRED)
            {
                showConsentForm();
            }
            else
            {
                int eventId = CONSENT_FORM_STATUS_UNKNOWN;

                switch(consentStatus)
                {
                    case ConsentInformation.ConsentStatus.NOT_REQUIRED:
                        eventId = CONSENT_FORM_STATUS_NOT_REQUIRED;
                        break;
                    case ConsentInformation.ConsentStatus.OBTAINED:
                        eventId = CONSENT_FORM_STATUS_OBTAINED;
                        break;
                }

                consentFormShowResult(eventId);
            }
        }

        @Override
        public void onConsentFormLoadFailure(FormError formError)
        {
            consentFormShowResult(CONSENT_FORM_LOAD_FAILURE);
        }

        @Override
        public void onConsentFormDismissed(@Nullable FormError formError)
        {
            consentFormShowResult(CONSENT_FORM_DISMISSED);
        }
    }

    private static final int CONSENT_FORM_INFO_UPDATE_FAILURE = 0;
    private static final int CONSENT_FORM_NOT_AVAILABLE = 1;
    private static final int CONSENT_FORM_STATUS_UNKNOWN = 2;
    private static final int CONSENT_FORM_STATUS_REQUIRED = 3;
    private static final int CONSENT_FORM_STATUS_NOT_REQUIRED = 4;
    private static final int CONSENT_FORM_STATUS_OBTAINED = 5;
    private static final int CONSENT_FORM_LOAD_FAILURE = 6;
    private static final int CONSENT_FORM_DISMISSED = 7;

    private static native void consentFormShowResult(int eventId);
}
