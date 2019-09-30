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
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import android.support.annotation.NonNull;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.Scopes;
import com.google.android.gms.common.SignInButton;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import java.io.InputStream;
import java.net.URL;

public class AndroidGoogleAccount
{
    private final Activity mActivityInstance;
    private GoogleSignInClient mGoogleSignInClient;

    public AndroidGoogleAccount(Activity ActivityInstance)
    {
        mActivityInstance = ActivityInstance;
        getSignInClient(ActivityInstance);
    }

    private void getSignInClient(Activity ActivityInstance)
    {
        GoogleSignInOptions SignInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                                                                   .requestEmail()
                                                                   .build();

        mGoogleSignInClient = GoogleSignIn.getClient(ActivityInstance, SignInOptions);
    }

    public boolean loadLastSignedInAccountInfo()
    {
        return loadLastSignedInAccountInfo(GoogleSignIn.getLastSignedInAccount(mActivityInstance));
    }

    public Intent getSignInIntent()
    {
        return mGoogleSignInClient.getSignInIntent();
    }

    public boolean signInIntentDataResult(Intent Data)
    {
        final Task<GoogleSignInAccount> SignInTask = GoogleSignIn.getSignedInAccountFromIntent(Data);

        try
        {
            loadLastSignedInAccountInfo(SignInTask.getResult(ApiException.class));
            return true;
        }
        catch(ApiException e)
        {
            if(e.getStatusCode() == GoogleSignInStatusCodes.DEVELOPER_ERROR)
            {
                Log.d("AndroidGoogleAccount", "DEVELOPER_ERROR -> Have you signed your project on Android console?");
            }
        }

        return false;
    }

    public void signOut()
    {
        mGoogleSignInClient.signOut().addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
    }

    public void revokeAccess()
    {
        mGoogleSignInClient.revokeAccess().addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
    }

    private boolean loadLastSignedInAccountInfo(final GoogleSignInAccount LastSignedInAccount)
    {
        if(LastSignedInAccount != null)
        {
            AccountInfo LastSignedInAccountInfo = new AccountInfo();
            final Uri PhotoUrl = LastSignedInAccount.getPhotoUrl();

            LastSignedInAccountInfo.id = LastSignedInAccount.getId();
            LastSignedInAccountInfo.displayName = LastSignedInAccount.getDisplayName();
            LastSignedInAccountInfo.email = LastSignedInAccount.getEmail();
            LastSignedInAccountInfo.familyName = LastSignedInAccount.getFamilyName();
            LastSignedInAccountInfo.givenName = LastSignedInAccount.getGivenName();

            if(PhotoUrl != null)
            {
                DownloadAccountPhotoTask DownloadAccountPhoto = new DownloadAccountPhotoTask(LastSignedInAccountInfo);
                DownloadAccountPhoto.execute(PhotoUrl.toString());
            }
            else
            {
                LastSignedInAccountInfo.photo = null;
                updateLastSignedInAccountInfo(LastSignedInAccountInfo);
            }

            return true;
        }

        return false;
    }

    private class DownloadAccountPhotoTask extends AsyncTask<String, Void, Bitmap>
    {
        private AccountInfo mLastSignedInAccountInfo;

        DownloadAccountPhotoTask(AccountInfo LastSignedInAccountInfo)
        {
            mLastSignedInAccountInfo = LastSignedInAccountInfo;
        }

        protected Bitmap doInBackground(String... urls)
        {
            final String PhotoUrl = urls[0];
            Bitmap AccountPhoto = null;

            try
            {
                InputStream PhotoStream = new java.net.URL(PhotoUrl).openStream();
                AccountPhoto = BitmapFactory.decodeStream(PhotoStream);
            }
            catch(Exception e)
            {
            }

            return AccountPhoto;
        }

        protected void onPostExecute(Bitmap AccountPhoto)
        {
            mLastSignedInAccountInfo.photo = AccountPhoto;
            updateLastSignedInAccountInfo(mLastSignedInAccountInfo);
        }
    }

    private class SignOutAccountListener implements OnCompleteListener<Void>
    {
        @Override
        public void onComplete(@NonNull Task<Void> task)
        {
            updateLastSignedInAccountInfo(null);
        }
    }

    public static class AccountInfo
    {
        public String id;
        public String displayName;
        public String email;
        public String familyName;
        public String givenName;
        public Bitmap photo;
    }

     private static native void updateLastSignedInAccountInfo(AccountInfo accountInfo);
}
