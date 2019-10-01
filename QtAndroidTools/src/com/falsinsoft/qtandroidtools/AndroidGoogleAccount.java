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
    private GoogleSignInClient mGoogleSignInClient = null;

    public AndroidGoogleAccount(Activity ActivityInstance)
    {
        mActivityInstance = ActivityInstance;
    }

    private GoogleSignInClient getSignInClient(String ScopeValue)
    {
        GoogleSignInOptions.Builder SignInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN);
        if(ScopeValue != null) SignInOptions.requestScopes(new Scope(ScopeValue));
        SignInOptions.requestEmail();

        return GoogleSignIn.getClient(mActivityInstance, SignInOptions.build());
    }

    public Intent getSignInIntent(int ScopeId)
    {
        Intent SignInIntent = null;

        if(mGoogleSignInClient == null)
        {
            mGoogleSignInClient = getSignInClient(ScopeIdToValue(ScopeId));
            SignInIntent = mGoogleSignInClient.getSignInIntent();
        }

        return SignInIntent;
    }

    public void signInIntentDataResult(Intent Data)
    {
        final Task<GoogleSignInAccount> SignInTask = GoogleSignIn.getSignedInAccountFromIntent(Data);
        boolean signInSuccessfully = true;

        try
        {
            loadSignedInAccountInfo(SignInTask.getResult(ApiException.class));
        }
        catch(ApiException e)
        {
            if(e.getStatusCode() == GoogleSignInStatusCodes.DEVELOPER_ERROR)
            {
                Log.d("AndroidGoogleAccount", "DEVELOPER_ERROR -> Have you signed your project on Android console?");
            }
            signInSuccessfully = false;
        }

        signedIn(signInSuccessfully);
    }

    public boolean signIn(int ScopeId)
    {
        if(mGoogleSignInClient == null)
        {
            Task<GoogleSignInAccount> SignInTask;

            mGoogleSignInClient = getSignInClient(ScopeIdToValue(ScopeId));

            SignInTask = mGoogleSignInClient.silentSignIn();
            if(SignInTask.isSuccessful())
            {
                loadSignedInAccountInfo(SignInTask.getResult());
                signedIn(true);
            }
            else
            {
                SignInTask.addOnCompleteListener(mActivityInstance, new SignInAccountListener());
            }

            return true;
        }

        return false;
    }

    public boolean signOut()
    {
        if(mGoogleSignInClient != null)
        {
            final Task<Void> SignOutTask = mGoogleSignInClient.signOut();

            if(SignOutTask.isSuccessful())
            {
                updateSignedInAccountInfo(null);
                mGoogleSignInClient = null;
                signedOut();
            }
            else
            {
                SignOutTask.addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
            }

            return true;
        }

        return false;
    }

    public boolean revokeAccess()
    {
        if(mGoogleSignInClient != null)
        {
            final Task<Void> SignOutTask = mGoogleSignInClient.revokeAccess();

            if(SignOutTask.isSuccessful())
            {
                updateSignedInAccountInfo(null);
                mGoogleSignInClient = null;
                signedOut();
            }
            else
            {
                SignOutTask.addOnCompleteListener(mActivityInstance, new SignOutAccountListener());
            }

            return true;
        }

        return false;
    }

    private boolean loadSignedInAccountInfo(final GoogleSignInAccount SignedInAccount)
    {
        if(SignedInAccount != null)
        {
            AccountInfo SignedInAccountInfo = new AccountInfo();
            final Uri PhotoUrl = SignedInAccount.getPhotoUrl();

            SignedInAccountInfo.id = SignedInAccount.getId();
            SignedInAccountInfo.displayName = SignedInAccount.getDisplayName();
            SignedInAccountInfo.email = SignedInAccount.getEmail();
            SignedInAccountInfo.familyName = SignedInAccount.getFamilyName();
            SignedInAccountInfo.givenName = SignedInAccount.getGivenName();

            if(PhotoUrl != null)
            {
                DownloadAccountPhotoTask DownloadAccountPhoto = new DownloadAccountPhotoTask(SignedInAccountInfo);
                DownloadAccountPhoto.execute(PhotoUrl.toString());
            }
            else
            {
                SignedInAccountInfo.photo = null;
                updateSignedInAccountInfo(SignedInAccountInfo);
            }

            return true;
        }

        return false;
    }

    private class DownloadAccountPhotoTask extends AsyncTask<String, Void, Bitmap>
    {
        private AccountInfo mSignedInAccountInfo;

        DownloadAccountPhotoTask(AccountInfo SignedInAccountInfo)
        {
            mSignedInAccountInfo = SignedInAccountInfo;
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
            mSignedInAccountInfo.photo = AccountPhoto;
            updateSignedInAccountInfo(mSignedInAccountInfo);
        }
    }

    private class SignInAccountListener implements OnCompleteListener<GoogleSignInAccount>
    {
        @Override
        public void onComplete(@NonNull Task<GoogleSignInAccount> SignInTask)
        {
            boolean signInSuccessfully = true;

            try
            {
                loadSignedInAccountInfo(SignInTask.getResult(ApiException.class));
            }
            catch(ApiException e)
            {
                if(e.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_REQUIRED)
                {
                    Log.d("AndroidGoogleAccount", "SIGN_IN_REQUIRED -> You have to signin by select account before use this call");
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
        public void onComplete(@NonNull Task<Void> SignOutTask)
        {
            updateSignedInAccountInfo(null);
            mGoogleSignInClient = null;
            signedOut();
        }
    }

    private String ScopeIdToValue(int ScopeId)
    {
        String ScopeValue = null;

        switch(ScopeId)
        {
            case SCOPE_APP_STATE:
                ScopeValue = Scopes.APP_STATE;
                break;
            case SCOPE_CLOUD_SAVE:
                ScopeValue = Scopes.CLOUD_SAVE;
                break;
            case SCOPE_DRIVE_APPFOLDER:
                ScopeValue = Scopes.CLOUD_SAVE;
                break;
            case SCOPE_DRIVE_FILE:
                ScopeValue = Scopes.DRIVE_FILE;
                break;
            case SCOPE_EMAIL:
                ScopeValue = Scopes.EMAIL;
                break;
            case SCOPE_FITNESS_ACTIVITY_READ:
                ScopeValue = Scopes.FITNESS_ACTIVITY_READ;
                break;
            case SCOPE_FITNESS_ACTIVITY_READ_WRITE:
                ScopeValue = Scopes.FITNESS_ACTIVITY_READ_WRITE;
                break;
            case SCOPE_FITNESS_BODY_READ:
                ScopeValue = Scopes.FITNESS_BODY_READ;
                break;
            case SCOPE_FITNESS_BODY_READ_WRITE:
                ScopeValue = Scopes.FITNESS_BODY_READ_WRITE;
                break;
            case SCOPE_FITNESS_LOCATION_READ:
                ScopeValue = Scopes.FITNESS_LOCATION_READ;
                break;
            case SCOPE_FITNESS_LOCATION_READ_WRITE:
                ScopeValue = Scopes.FITNESS_LOCATION_READ_WRITE;
                break;
            case SCOPE_FITNESS_NUTRITION_READ:
                ScopeValue = Scopes.FITNESS_NUTRITION_READ;
                break;
            case SCOPE_FITNESS_NUTRITION_READ_WRITE:
                ScopeValue = Scopes.FITNESS_NUTRITION_READ_WRITE;
                break;
            case SCOPE_GAMES:
                ScopeValue = Scopes.GAMES;
                break;
            case SCOPE_PLUS_ME:
                ScopeValue = Scopes.PLUS_ME;
                break;
            case SCOPE_PROFILE:
                ScopeValue = Scopes.PROFILE;
                break;
        }

        return ScopeValue;
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

    private static final int SCOPE_APP_STATE = 1;
    private static final int SCOPE_CLOUD_SAVE = 2;
    private static final int SCOPE_DRIVE_APPFOLDER = 3;
    private static final int SCOPE_DRIVE_FILE = 4;
    private static final int SCOPE_EMAIL = 5;
    private static final int SCOPE_FITNESS_ACTIVITY_READ = 6;
    private static final int SCOPE_FITNESS_ACTIVITY_READ_WRITE = 7;
    private static final int SCOPE_FITNESS_BODY_READ = 8;
    private static final int SCOPE_FITNESS_BODY_READ_WRITE = 9;
    private static final int SCOPE_FITNESS_LOCATION_READ = 10;
    private static final int SCOPE_FITNESS_LOCATION_READ_WRITE = 11;
    private static final int SCOPE_FITNESS_NUTRITION_READ = 12;
    private static final int SCOPE_FITNESS_NUTRITION_READ_WRITE = 13;
    private static final int SCOPE_GAMES = 14;
    private static final int SCOPE_PLUS_ME = 15;
    private static final int SCOPE_PROFILE = 16;

    private static native void updateSignedInAccountInfo(AccountInfo accountInfo);
    private static native void signedIn(boolean signInSuccessfully);
    private static native void signedOut();
}
