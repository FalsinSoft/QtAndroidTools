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
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.telephony.SignalStrength;
import android.util.Log;

public class AndroidSignalStrength
{
    private SignalStateListener mSignalStateListener;
    private final Activity mActivityInstance;

    private int mSignalStrength = 0;

    public AndroidSignalStrength(Context context)
    {
        mActivityInstance = (Activity)context;
        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mSignalStateListener = new SignalStateListener();
            }
        });
    }

    public int getSignalStrength()
    {
        return mSignalStrength;
    }

    public void appStateChanged(int newState)
    {
        final TelephonyManager telephonyManager = (TelephonyManager)mActivityInstance.getSystemService(Context.TELEPHONY_SERVICE);
        final SignalStateListener listener = mSignalStateListener;
        int listenEvent = PhoneStateListener.LISTEN_NONE;

        switch(newState)
        {
            case APP_STATE_CREATE:
            case APP_STATE_START:
                listenEvent = PhoneStateListener.LISTEN_SIGNAL_STRENGTHS;
                break;
            case APP_STATE_STOP:
            case APP_STATE_DESTROY:
                listenEvent = PhoneStateListener.LISTEN_NONE;
                break;
        }

        final int event = listenEvent;
        mActivityInstance.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                telephonyManager.listen(listener, event);
            }
        });
    }

    private class SignalStateListener extends PhoneStateListener
    {
        @Override
        public void onSignalStrengthsChanged(SignalStrength signalStrength)
        {
            super.onSignalStrengthsChanged(signalStrength);

            mSignalStrength = signalStrength.getGsmSignalStrength();
            signalStrengthChanged();
        }
    }

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static native void signalStrengthChanged();
}
