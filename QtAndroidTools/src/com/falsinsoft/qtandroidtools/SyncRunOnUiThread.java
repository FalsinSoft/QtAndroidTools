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

public class SyncRunOnUiThread
{
    private final Activity mActivityInstance;
	private final SyncRunOnUiThreadListener mCodeListener;
    private final Runnable mRunnableCode;

	public interface SyncRunOnUiThreadListener
	{
	   public void runOnUIThread();
	}	
	
    public SyncRunOnUiThread(Activity activityInstance, SyncRunOnUiThreadListener codeListener)
    {
        mActivityInstance = activityInstance;
		mCodeListener = codeListener;

        mRunnableCode = new Runnable()
        {
            public void run()
            {
                mCodeListener.runOnUIThread();

                synchronized(this)
                {
                    this.notify();
                }
            }
        };
    }

    public void exec()
    {
        synchronized(mRunnableCode)
        {
            mActivityInstance.runOnUiThread(mRunnableCode);
            try
            {
                mRunnableCode.wait();
            }
            catch( InterruptedException e)
            {
                e.printStackTrace();
            }
        }
    }
}