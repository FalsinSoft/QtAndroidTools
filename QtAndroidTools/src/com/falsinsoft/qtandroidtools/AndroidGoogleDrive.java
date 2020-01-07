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
import android.provider.MediaStore;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.common.api.Scope;
import com.google.api.client.extensions.android.http.AndroidHttp;
import com.google.api.client.googleapis.extensions.android.gms.auth.UserRecoverableAuthIOException;
import com.google.api.client.googleapis.extensions.android.gms.auth.GoogleAccountCredential;
import com.google.api.client.json.gson.GsonFactory;
import com.google.api.services.drive.Drive;
import com.google.api.services.drive.DriveScopes;
import com.google.api.services.drive.model.File;
import com.google.api.services.drive.model.FileList;

import java.util.Collections;
import java.util.List;
import java.io.IOException;

public class AndroidGoogleDrive
{
    private static final String TAG = "AndroidGoogleDrive";
    private final Activity mActivityInstance;
    private Drive mDriveService = null;

    public AndroidGoogleDrive(Activity ActivityInstance)
    {
        mActivityInstance = ActivityInstance;
    }

    public boolean authenticate(String AppName, String ScopeName)
    {
        final GoogleSignInAccount SignInAccount = GoogleSignIn.getLastSignedInAccount(mActivityInstance);

        if(SignInAccount != null)
        {
            GoogleAccountCredential AccountCredential;
            Drive.Builder DriveBuilder;

            AccountCredential = GoogleAccountCredential.usingOAuth2(mActivityInstance, Collections.singleton(ScopeName));
            AccountCredential.setSelectedAccount(SignInAccount.getAccount());

            DriveBuilder = new Drive.Builder(AndroidHttp.newCompatibleTransport(), new GsonFactory(), AccountCredential);
            DriveBuilder.setApplicationName(AppName);
            mDriveService = DriveBuilder.build();

            return true;
        }

        Log.d(TAG, "You have to signin by select account before use this call!");
        return false;
    }

    public boolean isAuthenticated()
    {
        return (mDriveService != null) ? true : false;
    }

    public DriveFile[] listFiles(String Query)
    {
        if(mDriveService != null)
        {
            DriveFile[] DriveFileList;
            File[] FileList;

            try
            {
                FileList = mDriveService.files()
                                        .list()
                                        .setQ(Query)
                                        .setSpaces("drive")
                                        .setFields("files(id, name, mimeType, parents)")
                                        .execute()
                                        .getFiles()
                                        .toArray(new File[0]);
            }
            catch(UserRecoverableAuthIOException e)
            {
                Log.d(TAG, "Authorization scope not requested at signin!");
                return null;
            }
            catch(IOException e)
            {
                return null;
            }

            DriveFileList = new DriveFile[FileList.length];
            for(int i = 0; i < FileList.length; i++)
            {
                DriveFile FileData = new DriveFile();
                final File FileInfo = FileList[i];

                FileData.id = FileInfo.getId();
                FileData.name = FileInfo.getName();
                FileData.mimeType = FileInfo.getMimeType();
                FileData.parents = (FileInfo.getParents() != null) ? FileInfo.getParents().toArray(new String[0]) : null;

                DriveFileList[i] = FileData;
            }

            return DriveFileList;
        }

        return null;
    }

    public String getRootId()
    {
        if(mDriveService != null)
        {
            File FileInfo;

            try
            {
                FileInfo = mDriveService.files()
                                        .get("root")
                                        .execute();
            }
            catch(UserRecoverableAuthIOException e)
            {
                Log.d(TAG, "Authorization scope not requested at signin!");
                return null;
            }
            catch(IOException e)
            {
                return null;
            }

            return FileInfo.getId();
        }

        return null;
    }

    public static class DriveFile
    {
        public String id;
        public String name;
        public String mimeType;
        public String[] parents;
    }
}
