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
import com.google.api.client.googleapis.media.MediaHttpDownloaderProgressListener;
import com.google.api.client.googleapis.media.MediaHttpDownloader;
import com.google.api.client.googleapis.media.MediaHttpUploaderProgressListener;
import com.google.api.client.googleapis.media.MediaHttpUploader;
import com.google.api.client.json.gson.GsonFactory;
import com.google.api.client.http.FileContent;
import com.google.api.services.drive.Drive;
import com.google.api.services.drive.DriveScopes;
import com.google.api.services.drive.model.File;
import com.google.api.services.drive.model.FileList;

import java.util.Collections;
import java.util.List;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.FileOutputStream;

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
                Log.d(TAG, e.toString());
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
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return FileInfo.getId();
        }

        return null;
    }

    public String createFolder(String Name, String ParentFolderId)
    {
        if(mDriveService != null)
        {
            File FolderMetadata = new File();
            File FolderData;

            FolderMetadata.setName(Name);
            FolderMetadata.setMimeType("application/vnd.google-apps.folder");
            if(!ParentFolderId.isEmpty()) FolderMetadata.setParents(Collections.singletonList(ParentFolderId));

            try
            {
                FolderData = mDriveService.files()
                                          .create(FolderMetadata)
                                          .setFields("id")
                                          .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return FolderData.getId();
        }

        return null;
    }

    public File getFileMetadata(String FileId, String Fields)
    {
        if(mDriveService != null)
        {
            File FileInfo;

            try
            {
                FileInfo = mDriveService.files()
                                        .get(FileId)
                                        .setFields(Fields)
                                        .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return FileInfo;
        }

        return null;
    }

    public boolean moveFile(String FileId, String FolderId)
    {
        if(mDriveService != null)
        {
            StringBuilder PreviousParents = new StringBuilder();
            File ParentData, FileData;

            try
            {
                ParentData = mDriveService.files()
                                          .get(FileId)
                                          .setFields("parents")
                                          .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return false;
            }

            for(String ParentId : ParentData.getParents())
            {
                PreviousParents.append(ParentId);
                PreviousParents.append(',');
            }

            try
            {
                FileData = mDriveService.files()
                                        .update(FileId, null)
                                        .setAddParents(FolderId)
                                        .setRemoveParents(PreviousParents.toString())
                                        .setFields("id, parents")
                                        .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return false;
            }

            return true;
        }

        return false;
    }

    public boolean deleteFile(String FileId)
    {
        if(mDriveService != null)
        {
            try
            {
                mDriveService.files()
                             .delete(FileId)
                             .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return false;
            }

            return true;
        }

        return false;
    }

    public boolean downloadFile(String FileId, String LocalFilePath)
    {
        if(mDriveService != null)
        {
            try
            {
                Drive.Files.Get FileDownloadRequest = mDriveService.files().get(FileId);
                FileDownloadRequest.getMediaHttpDownloader().setProgressListener(new FileDownloadProgressListener());
                FileDownloadRequest.executeMediaAndDownloadTo(new FileOutputStream(LocalFilePath));
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return false;
            }

            return true;
        }

        return false;
    }

    public String uploadFile(String LocalFilePath, String Name, String MimeType, String ParentFolderId)
    {
        if(mDriveService != null)
        {
            java.io.File MediaFile = new java.io.File(LocalFilePath);
            File FileMetadata = new File();
            File UploadedFileData;

            FileMetadata.setName(Name);
            if(!ParentFolderId.isEmpty()) FileMetadata.setParents(Collections.singletonList(ParentFolderId));

            try
            {
                Drive.Files.Create FileUploadRequest = mDriveService.files().create(FileMetadata, new FileContent(MimeType, MediaFile));
                FileUploadRequest.getMediaHttpUploader().setProgressListener(new FileUploadProgressListener());
                UploadedFileData = FileUploadRequest.setFields("id").execute();
            }
            catch(FileNotFoundException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return UploadedFileData.getId();
        }

        return null;
    }

    private class FileDownloadProgressListener implements MediaHttpDownloaderProgressListener
    {
        public void progressChanged(MediaHttpDownloader downloader)
        {
            switch(downloader.getDownloadState())
            {
                case MEDIA_IN_PROGRESS:
                    downloadProgressChanged(STATE_MEDIA_IN_PROGRESS, downloader.getProgress());
                    break;
                case MEDIA_COMPLETE:
                    downloadProgressChanged(STATE_MEDIA_COMPLETE, 1.0);
                    break;
            }
        }
    }

    private class FileUploadProgressListener implements MediaHttpUploaderProgressListener
    {
        public void progressChanged(MediaHttpUploader uploader) throws IOException
        {
            switch (uploader.getUploadState())
            {
                case INITIATION_STARTED:
                    uploadProgressChanged(STATE_INITIATION_STARTED, 0.0);
                    break;
                case INITIATION_COMPLETE:
                    uploadProgressChanged(STATE_INITIATION_COMPLETE, 0.0);
                    break;
                case MEDIA_IN_PROGRESS:
                    uploadProgressChanged(STATE_MEDIA_IN_PROGRESS, uploader.getProgress());
                    break;
                case MEDIA_COMPLETE:
                    uploadProgressChanged(STATE_MEDIA_COMPLETE, 1.0);
                    break;
            }
        }
    }

    public static class DriveFile
    {
        public String id;
        public String name;
        public String mimeType;
        public String[] parents;
    }

    private static final int STATE_INITIATION_STARTED = 0;
    private static final int STATE_INITIATION_COMPLETE = 1;
    private static final int STATE_MEDIA_IN_PROGRESS = 2;
    private static final int STATE_MEDIA_COMPLETE = 3;

    private static native void downloadProgressChanged(int state, double progress);
    private static native void uploadProgressChanged(int state, double progress);
}
