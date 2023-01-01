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
    private int mConsentStatus = ConsentInformation.ConsentStatus.UNKNOWN;
    private ConsentForm mConsentForm = null;

    public AndroidUserMessagingPlatform(Activity activityInstance)
    {
        mConsentInformation = UserMessagingPlatform.getConsentInformation(activityInstance);
        mConsentListener = new ConsentListener();
        mActivityInstance = activityInstance;
    }

    public boolean showConsentForm()
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

            return true;
        }

        return false;
    }

    public void resetConsentInformation()
    {
        mConsentInformation.reset();
    }

    public void requestConsentForm()
    {
        final ConsentRequestParameters params = new ConsentRequestParameters.Builder().build();
        mConsentInformation.requestConsentInfoUpdate(mActivityInstance, params, mConsentListener, mConsentListener);
    }

    public int consentStatus()
    {
        int status = CONSENT_FORM_STATUS_UNKNOWN;

        switch(mConsentStatus)
        {
            case ConsentInformation.ConsentStatus.REQUIRED:
                status = CONSENT_FORM_STATUS_REQUIRED;
                break;
            case ConsentInformation.ConsentStatus.NOT_REQUIRED:
                status = CONSENT_FORM_STATUS_NOT_REQUIRED;
                break;
            case ConsentInformation.ConsentStatus.OBTAINED:
                status = CONSENT_FORM_STATUS_OBTAINED;
                break;
            case ConsentInformation.ConsentStatus.UNKNOWN:
                status = CONSENT_FORM_STATUS_UNKNOWN;
                break;
        }

        return status;
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
                consentFormRequestResult(CONSENT_FORM_NOT_AVAILABLE);
            }
        }

        @Override
        public void onConsentInfoUpdateFailure(FormError formError)
        {
            consentFormRequestResult(CONSENT_FORM_INFO_UPDATE_FAILURE);
        }

        @Override
        public void onConsentFormLoadSuccess(ConsentForm consentForm)
        {
            mConsentStatus = mConsentInformation.getConsentStatus();
            mConsentForm = consentForm;

            consentFormRequestResult(CONSENT_FORM_LOAD_SUCCESS);
        }

        @Override
        public void onConsentFormLoadFailure(FormError formError)
        {
            consentFormRequestResult(CONSENT_FORM_LOAD_FAILURE);
        }

        @Override
        public void onConsentFormDismissed(@Nullable FormError formError)
        {
            consentFormClosed();
        }
    }

    private static final int CONSENT_FORM_INFO_UPDATE_FAILURE = 0;
    private static final int CONSENT_FORM_NOT_AVAILABLE = 1;
    private static final int CONSENT_FORM_LOAD_SUCCESS = 2;
    private static final int CONSENT_FORM_LOAD_FAILURE = 3;

    private static final int CONSENT_FORM_STATUS_UNKNOWN = 0;
    private static final int CONSENT_FORM_STATUS_REQUIRED = 1;
    private static final int CONSENT_FORM_STATUS_NOT_REQUIRED = 2;
    private static final int CONSENT_FORM_STATUS_OBTAINED = 3;

    private static native void consentFormRequestResult(int eventId);
    private static native void consentFormClosed();
}
