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
import android.util.Log;
import android.os.Build;
import android.media.AudioManager;
import android.media.AudioFocusRequest;
import android.media.AudioAttributes;
import android.media.AudioManager.OnAudioFocusChangeListener;

public class AndroidAudio
{
    private static final String TAG = "AndroidAudio";
    private final Activity mActivityInstance;
    private final AudioManager mAudioManager;
    private AudioFocusChangeListener mAudioFocusChangeListener = null;

    public AndroidAudio(Activity activityInstance)
    {
        mAudioManager = (AudioManager) activityInstance.getSystemService(Context.AUDIO_SERVICE);
        mActivityInstance = activityInstance;
    }

    public boolean requestFocus()
    {
        if(mAudioFocusChangeListener == null)
        {
            mAudioFocusChangeListener = new AudioFocusChangeListener();
            int result;

            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                result = mAudioManager.requestAudioFocus(createAudioFocusRequest(mAudioFocusChangeListener));
            else
                result = mAudioManager.requestAudioFocus(mAudioFocusChangeListener, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);

            if(result == AudioManager.AUDIOFOCUS_REQUEST_FAILED)
            {
                mAudioFocusChangeListener = null;
                return false;
            }
        }

        return true;
    }

    public void abandonFocus()
    {
        if(mAudioFocusChangeListener != null)
        {
            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                mAudioManager.abandonAudioFocusRequest(createAudioFocusRequest(mAudioFocusChangeListener));
            else
                mAudioManager.abandonAudioFocus(mAudioFocusChangeListener);

            mAudioFocusChangeListener = null;
        }
    }

    private AudioFocusRequest createAudioFocusRequest(AudioFocusChangeListener listener)
    {
        AudioAttributes audioAttributes;
        AudioFocusRequest focusRequest;

        audioAttributes = new AudioAttributes.Builder()
                                             .setUsage(AudioAttributes.USAGE_MEDIA)
                                             .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                                             .build();
        focusRequest = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN)
                                            .setAudioAttributes(audioAttributes)
                                            .setAcceptsDelayedFocusGain(true)
                                            .setWillPauseWhenDucked(true)
                                            .setOnAudioFocusChangeListener(listener)
                                            .build();
        return focusRequest;
    }

    private class AudioFocusChangeListener implements OnAudioFocusChangeListener
    {
        @Override
        public void onAudioFocusChange(int focusChange)
        {
            switch(focusChange)
            {
                case AudioManager.AUDIOFOCUS_GAIN:
                    focusChanged(true);
                    break;
                case AudioManager.AUDIOFOCUS_LOSS:
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                    focusChanged(false);
                    break;
            }
        }
    }

    private static native void focusChanged(boolean focus);
}
