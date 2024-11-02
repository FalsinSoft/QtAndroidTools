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
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.DialogInterface;
import android.provider.Settings;
import android.os.Build;
import android.util.Log;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.hardware.biometrics.BiometricManager;
import android.hardware.biometrics.BiometricPrompt;

import java.util.concurrent.Executor;

public class AndroidAuthentication
{
    private static final String TAG = "AndroidAuthentication";
    private final Activity mActivityInstance;

    private int mAuthenticators = 0;
    private String mTitle = new String();
    private String mDescription = new String();
    private String mNegativeButton = new String();
    private CancellationSignal mCancellationSignal = new CancellationSignal();

    public AndroidAuthentication(Context context)
    {
        mActivityInstance = (Activity)context;
        mCancellationSignal.setOnCancelListener(new CancellationSignalListener());
    }

    public void setAuthenticators(int authenticators)
    {
        mAuthenticators = 0;
        if((authenticators & BIOMETRIC_STRONG) == BIOMETRIC_STRONG) mAuthenticators |= BiometricManager.Authenticators.BIOMETRIC_STRONG;
        if((authenticators & BIOMETRIC_WEAK) == BIOMETRIC_WEAK) mAuthenticators |= BiometricManager.Authenticators.BIOMETRIC_WEAK;
        if((authenticators & DEVICE_CREDENTIAL) == DEVICE_CREDENTIAL) mAuthenticators |= BiometricManager.Authenticators.DEVICE_CREDENTIAL;
    }

    public void setTitle(String title)
    {
        mTitle = title;
    }

    public void setDescription(String description)
    {
        mDescription = description;
    }

    public void setNegativeButton(String negativeButton)
    {
        mNegativeButton = negativeButton;
    }

    public void cancel()
    {
        mCancellationSignal.cancel();
    }

    public int canAuthenticate()
    {
        final BiometricManager biometricManager = mActivityInstance.getSystemService(BiometricManager.class);
        int result = BIOMETRIC_STATUS_UNKNOWN;

        switch(biometricManager.canAuthenticate(mAuthenticators))
        {
            case BiometricManager.BIOMETRIC_SUCCESS:
                result = BIOMETRIC_SUCCESS;
                break;
            case BiometricManager.BIOMETRIC_ERROR_NO_HARDWARE:
                result = BIOMETRIC_ERROR_NO_HARDWARE;
                break;
            case BiometricManager.BIOMETRIC_ERROR_HW_UNAVAILABLE:
                result = BIOMETRIC_ERROR_HW_UNAVAILABLE;
                break;
            case BiometricManager.BIOMETRIC_ERROR_NONE_ENROLLED:
                result = BIOMETRIC_ERROR_NONE_ENROLLED;
                break;
            case BiometricManager.BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED:
                result = BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED;
                break;
        }

        return result;
    }

    public boolean requestBiometricEnroll()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
        {
            final Intent enrollIntent = new Intent(Settings.ACTION_BIOMETRIC_ENROLL);
            enrollIntent.putExtra(Settings.EXTRA_BIOMETRIC_AUTHENTICATORS_ALLOWED, mAuthenticators);
            mActivityInstance.startActivityForResult(enrollIntent, 123456);
            return true;
        }

        Log.d(TAG, "Could not request biometric enrollment in API level < 30");
        return false;
    }

    public boolean authenticate()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            final BiometricPrompt.Builder builder = new BiometricPrompt.Builder(mActivityInstance);
            final Executor executor = mActivityInstance.getMainExecutor();

            builder.setTitle(mTitle);
            builder.setDescription(mDescription);
            builder.setConfirmationRequired(false);
            builder.setNegativeButton(mNegativeButton, executor, new NegativeButtonListener());
            builder.build().authenticate(mCancellationSignal,
                                         executor,
                                         new BiometricAuthenticationCallback());
            return true;
        }

        Log.d(TAG, "Could not request biometric authentication in API level < 28");
        return false;
    }

    private class BiometricAuthenticationCallback extends BiometricPrompt.AuthenticationCallback
    {
        @Override
        public void onAuthenticationError(int errorCode, CharSequence errString)
        {
            super.onAuthenticationError(errorCode, errString);
            authenticationError(errorCode, errString.toString());
        }

        @Override
        public void onAuthenticationSucceeded(BiometricPrompt.AuthenticationResult result)
        {
            super.onAuthenticationSucceeded(result);
            authenticationSucceeded();
        }

        @Override
        public void onAuthenticationFailed()
        {
            super.onAuthenticationFailed();
            authenticationFailed();
        }
    }

    private class CancellationSignalListener implements CancellationSignal.OnCancelListener
    {
        @Override
        public void onCancel()
        {
            authenticationCancelled();
        }
    }

    private class NegativeButtonListener implements DialogInterface.OnClickListener
    {
        @Override
        public void onClick(DialogInterface dialog, int which)
        {
            authenticationCancelled();
        }
    }

    private final int BIOMETRIC_STRONG = 0x01;
    private final int BIOMETRIC_WEAK = 0x02;
    private final int DEVICE_CREDENTIAL = 0x04;

    private final int BIOMETRIC_STATUS_UNKNOWN = 0;
    private final int BIOMETRIC_SUCCESS = 1;
    private final int BIOMETRIC_ERROR_NO_HARDWARE = 2;
    private final int BIOMETRIC_ERROR_HW_UNAVAILABLE = 3;
    private final int BIOMETRIC_ERROR_NONE_ENROLLED = 4;
    private final int BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED = 5;

    private static native void authenticationError(int errorCode, String errString);
    private static native void authenticationSucceeded();
    private static native void authenticationFailed();
    private static native void authenticationCancelled();
}
