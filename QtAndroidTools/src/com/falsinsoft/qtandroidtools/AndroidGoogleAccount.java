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
import androidx.annotation.NonNull;
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
    private static final String TAG = "AndroidGoogleAccount";
    private final Activity mActivityInstance;
    private GoogleSignInClient mGoogleSignInClient = null;

    public AndroidGoogleAccount(Activity activityInstance)
    {
        mActivityInstance = activityInstance;
    }

    private GoogleSignInClient getSignInClient(String scopeName)
    {
        GoogleSignInOptions.Builder signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN);
        if(scopeName.isEmpty() == false) signInOptions.requestScopes(new Scope(scopeName));
        signInOptions.requestEmail();

        return GoogleSignIn.getClient(mActivityInstance, signInOptions.build());
    }

    public Intent getSignInIntent(String scopeName)
    {
        Intent signInIntent = null;

        if(mGoogleSignInClient == null)
        {
            mGoogleSignInClient = getSignInClient(scopeName);
            signInIntent = mGoogleSignInClient.getSignInIntent();
        }

        return signInIntent;
    }

    public void signInIntentDataResult(Intent data)
    {
        final Task<GoogleSignInAccount> signInTask = GoogleSignIn.getSignedInAccountFromIntent(data);

        if(signInTask.isSuccessful())
        {
            loadSignedInAccountInfo(signInTask.getResult());
            signedIn(true);
        }
        else
        {
            signInTask.addOnCompleteListener(mActivityInstance, new SignInAccountListener());
        }
    }

    public boolean signIn(String scopeName)
    {
        if(mGoogleSignInClient == null)
        {
            Task<GoogleSignInAccount> signInTask;

            mGoogleSignInClient = getSignInClient(scopeName);

            signInTask = mGoogleSignInClient.silentSignIn();
            if(signInTask.isSuccessful())
            {
                loadSignedInAccountInfo(signInTask.getResult());
                signedIn(true);
            }
            else
            {
                signInTask.addOnCompleteListener(mActivityInstance, new SignInAccountListener());
            }

            return true;
        }

        return false;
    }

    public boolean signOut()
    {
        if(mGoogleSignInClient != null)
        {
            final Task<Void> signOutTask = mGoogleSignInClient.signOut();

            if(signOutTask.isSuccessful())
            {
                updateSignedInAccountInfo(null);
                mGoogleSignInClient = null;
                signedOut();
            }
            else
            {
                signOutTask.addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
            }

            return true;
        }

        return false;
    }

    public boolean revokeAccess()
    {
        if(mGoogleSignInClient != null)
        {
            final Task<Void> signOutTask = mGoogleSignInClient.revokeAccess();

            if(signOutTask.isSuccessful())
            {
                updateSignedInAccountInfo(null);
                mGoogleSignInClient = null;
                signedOut();
            }
            else
            {
                signOutTask.addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
            }

            return true;
        }

        return false;
    }

    private boolean loadSignedInAccountInfo(final GoogleSignInAccount signedInAccount)
    {
        if(signedInAccount != null)
        {
            AccountInfo signedInAccountInfo = new AccountInfo();
            final Uri photoUrl = signedInAccount.getPhotoUrl();

            signedInAccountInfo.id = signedInAccount.getId();
            signedInAccountInfo.displayName = signedInAccount.getDisplayName();
            signedInAccountInfo.email = signedInAccount.getEmail();
            signedInAccountInfo.familyName = signedInAccount.getFamilyName();
            signedInAccountInfo.givenName = signedInAccount.getGivenName();

            if(photoUrl != null)
            {
                DownloadAccountPhotoTask downloadAccountPhoto = new DownloadAccountPhotoTask(signedInAccountInfo);
                downloadAccountPhoto.execute(photoUrl.toString());
            }
            else
            {
                signedInAccountInfo.photo = null;
                updateSignedInAccountInfo(signedInAccountInfo);
            }

            return true;
        }

        return false;
    }

    private class DownloadAccountPhotoTask extends AsyncTask<String, Void, Bitmap>
    {
        private AccountInfo mSignedInAccountInfo;

        DownloadAccountPhotoTask(AccountInfo signedInAccountInfo)
        {
            mSignedInAccountInfo = signedInAccountInfo;
        }

        protected Bitmap doInBackground(String... urls)
        {
            final String photoUrl = urls[0];
            Bitmap accountPhoto = null;

            try
            {
                InputStream photoStream = new java.net.URL(photoUrl).openStream();
                accountPhoto = BitmapFactory.decodeStream(photoStream);
            }
            catch(Exception e)
            {
            }

            return accountPhoto;
        }

        protected void onPostExecute(Bitmap accountPhoto)
        {
            mSignedInAccountInfo.photo = accountPhoto;
            updateSignedInAccountInfo(mSignedInAccountInfo);
        }
    }

    private class SignInAccountListener implements OnCompleteListener<GoogleSignInAccount>
    {
        @Override
        public void onComplete(@NonNull Task<GoogleSignInAccount> signInTask)
        {
            boolean signInSuccessfully = true;

            try
            {
                loadSignedInAccountInfo(signInTask.getResult(ApiException.class));
            }
            catch(ApiException e)
            {
                switch(e.getStatusCode())
                {
                    case GoogleSignInStatusCodes.DEVELOPER_ERROR:
                        Log.d(TAG, "DEVELOPER_ERROR -> Have you signed your project on Android console?");
                        break;
                    case GoogleSignInStatusCodes.SIGN_IN_REQUIRED:
                        Log.d(TAG, "SIGN_IN_REQUIRED -> You have to signin by select account before use this call");
                        break;
                }
                signInSuccessfully = false;
                mGoogleSignInClient = null;
            }

            signedIn(signInSuccessfully);
        }
    }

    private class SignOutAccountListener implements OnCompleteListener<Void>
    {
        @Override
        public void onComplete(@NonNull Task<Void> signOutTask)
        {
            updateSignedInAccountInfo(null);
            mGoogleSignInClient = null;
            signedOut();
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

    private static native void updateSignedInAccountInfo(AccountInfo accountInfo);
    private static native void signedIn(boolean signInSuccessfully);
    private static native void signedOut();
}
