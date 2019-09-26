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
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.Scopes;
import com.google.android.gms.common.SignInButton;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import java.io.FileNotFoundException;
import java.io.IOException;

public class AndroidGoogleAccount
{
    private final Activity mActivityInstance;
    private GoogleSignInClient mGoogleSignInClient;
    private GoogleSignInAccount mLastSignedInAccount;

    public AndroidGoogleAccount(Activity ActivityInstance)
    {
        mLastSignedInAccount = GoogleSignIn.getLastSignedInAccount(ActivityInstance);
        mActivityInstance = ActivityInstance;
        getSignInClient(ActivityInstance);
    }

    private void getSignInClient(Activity ActivityInstance)
    {
        GoogleSignInOptions SignInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                                                                   .requestProfile()
                                                                   .requestEmail()
                                                                   .build();

        mGoogleSignInClient = GoogleSignIn.getClient(ActivityInstance, SignInOptions);
    }

    public AccountInfo getLastSignedInAccountInfo()
    {
        AccountInfo Account = null;

        if(mLastSignedInAccount != null)
        {
            final Uri PhotoUrl = mLastSignedInAccount.getPhotoUrl();
            Account = new AccountInfo();

            Account.id = mLastSignedInAccount.getId();
            Account.displayName = mLastSignedInAccount.getDisplayName();
            Account.email = mLastSignedInAccount.getEmail();
            Account.familyName = mLastSignedInAccount.getFamilyName();
            Account.givenName = mLastSignedInAccount.getGivenName();
        }

        return Account;
    }

    public Intent getSignInIntent()
    {
        return mGoogleSignInClient.getSignInIntent();
    }

    public void signInIntentDataResult(Intent Data)
    {
        Task<GoogleSignInAccount> SignInTask = GoogleSignIn.getSignedInAccountFromIntent(Data);

        if(SignInTask.isSuccessful())
        {
            try
            {
                mLastSignedInAccount = SignInTask.getResult(ApiException.class);
            }
            catch(ApiException e)
            {
            }
        }
    }

    public static class AccountInfo
    {
        public String id;
        public String displayName;
        public String email;
        public String familyName;
        public String givenName;
        public String photoUrl;
    }
}
