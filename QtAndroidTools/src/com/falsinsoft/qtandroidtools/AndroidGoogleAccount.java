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
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.Scopes;
import com.google.android.gms.common.SignInButton;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

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
                                                                   .requestEmail()
                                                                   .build();

        mGoogleSignInClient = GoogleSignIn.getClient(ActivityInstance, SignInOptions);
    }

    public AccountInfo getLastSignedInAccountInfo()
    {
        AccountInfo Account = null;

        if(mLastSignedInAccount != null)
        {
            Uri PhotoUrl;

            Account = new AccountInfo();
            Account.id = mLastSignedInAccount.getId();
            Account.displayName = mLastSignedInAccount.getDisplayName();
            Account.email = mLastSignedInAccount.getEmail();
            Account.familyName = mLastSignedInAccount.getFamilyName();
            Account.givenName = mLastSignedInAccount.getGivenName();

            PhotoUrl = mLastSignedInAccount.getPhotoUrl();
            if(PhotoUrl != null)
                Account.photoUrl = PhotoUrl.toString();
            else
                Account.photoUrl = new String();
        }

        return Account;
    }

    public Intent getSignInIntent()
    {
        return mGoogleSignInClient.getSignInIntent();
    }

    public boolean signInIntentDataResult(Intent Data)
    {
        Task<GoogleSignInAccount> SignInTask = GoogleSignIn.getSignedInAccountFromIntent(Data);

        if(SignInTask.isSuccessful())
        {
            try
            {
                mLastSignedInAccount = SignInTask.getResult(ApiException.class);
                return true;
            }
            catch(ApiException e)
            {
                if(e.getStatusCode() == GoogleSignInStatusCodes.DEVELOPER_ERROR)
                {
                    Log.d("AndroidGoogleAccount", "DEVELOPER_ERROR -> Have you signed your project on Android console?");
                }
            }
        }

        return false;
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
