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
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.Intent;
import android.os.BatteryManager;
import android.content.Context;

public class AndroidBatteryState
{
    private final BatteryStateChangeReceiver mBatteryStateChangeReceiver;
    private final IntentFilter mBatteryStateFilter;
    private final Activity mActivityInstance;

    private boolean mReceiverRegistered = false;
    private int mLevel = 0;
    private boolean mOnCharge = false;

    public AndroidBatteryState(Activity ActivityInstance)
    {
        mBatteryStateChangeReceiver = new BatteryStateChangeReceiver();
        mBatteryStateFilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        mActivityInstance = ActivityInstance;
    }

    public int getLevel()
    {
        return mLevel;
    }

    public boolean isOnCharge()
    {
        return mOnCharge;
    }

    public void appStateChanged(int newState)
    {
        switch(newState)
        {
            case APP_STATE_CREATE:
            case APP_STATE_START:
                if(mReceiverRegistered == false)
                {
                    if(mActivityInstance.registerReceiver(mBatteryStateChangeReceiver, mBatteryStateFilter) != null)
                    {
                        mReceiverRegistered = true;
                    }
                }
                break;
            case APP_STATE_STOP:
            case APP_STATE_DESTROY:
                if(mReceiverRegistered == true)
                {
                    mActivityInstance.unregisterReceiver(mBatteryStateChangeReceiver);
                    mReceiverRegistered = false;
                }
                break;
        }
    }

    private class BatteryStateChangeReceiver extends BroadcastReceiver
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            final boolean CurrentOnCharge = (intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1) == 0) ? false : true;
            final int Level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
            final int Scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

            if(Level >= 0 && Scale > 0)
            {
                final int CurrentLevel = ((Level * 100) / Scale);
                if(CurrentLevel != mLevel)
                {
                    mLevel = CurrentLevel;
                    batteryLevelChanged();
                }
            }
            if(CurrentOnCharge != mOnCharge)
            {
                mOnCharge = CurrentOnCharge;
                batteryOnChargeChanged();
            }
        }
    }

    private static final int APP_STATE_CREATE = 0;
    private static final int APP_STATE_START = 1;
    private static final int APP_STATE_STOP = 2;
    private static final int APP_STATE_DESTROY = 3;

    private static native void batteryLevelChanged();
    private static native void batteryOnChargeChanged();
}
