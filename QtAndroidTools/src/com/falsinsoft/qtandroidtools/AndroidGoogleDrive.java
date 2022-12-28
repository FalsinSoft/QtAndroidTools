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

    public AndroidGoogleDrive(Activity activityInstance)
    {
        mActivityInstance = activityInstance;
    }

    public boolean authenticate(String appName, String scopeName)
    {
        final GoogleSignInAccount signInAccount = GoogleSignIn.getLastSignedInAccount(mActivityInstance);

        if(signInAccount != null)
        {
            GoogleAccountCredential accountCredential;
            Drive.Builder driveBuilder;

            accountCredential = GoogleAccountCredential.usingOAuth2(mActivityInstance, Collections.singleton(scopeName));
            accountCredential.setSelectedAccount(signInAccount.getAccount());

            driveBuilder = new Drive.Builder(AndroidHttp.newCompatibleTransport(), new GsonFactory(), accountCredential);
            driveBuilder.setApplicationName(appName);
            mDriveService = driveBuilder.build();

            return true;
        }

        Log.d(TAG, "You have to signin by select account before use this call!");
        return false;
    }

    public boolean isAuthenticated()
    {
        return (mDriveService != null) ? true : false;
    }

    public DriveFile[] listFiles(String query, String spaces)
    {
        if(mDriveService != null)
        {
            DriveFile[] driveFileList;
            File[] fileList;

            try
            {
                fileList = mDriveService.files()
                                        .list()
                                        .setQ(query)
                                        .setSpaces(spaces)
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

            driveFileList = new DriveFile[fileList.length];
            for(int i = 0; i < fileList.length; i++)
            {
                DriveFile fileData = new DriveFile();
                final File fileInfo = fileList[i];

                fileData.id = fileInfo.getId();
                fileData.name = fileInfo.getName();
                fileData.mimeType = fileInfo.getMimeType();
                fileData.parents = (fileInfo.getParents() != null) ? fileInfo.getParents().toArray(new String[0]) : null;

                driveFileList[i] = fileData;
            }

            return driveFileList;
        }

        return null;
    }

    public String getRootId()
    {
        if(mDriveService != null)
        {
            File fileInfo;

            try
            {
                fileInfo = mDriveService.files()
                                        .get("root")
                                        .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return fileInfo.getId();
        }

        return null;
    }

    public String createFolder(String name, String parentFolderId)
    {
        if(mDriveService != null)
        {
            File folderMetadata = new File();
            File folderData;

            folderMetadata.setName(name);
            folderMetadata.setMimeType("application/vnd.google-apps.folder");
            if(!parentFolderId.isEmpty()) folderMetadata.setParents(Collections.singletonList(parentFolderId));

            try
            {
                folderData = mDriveService.files()
                                          .create(folderMetadata)
                                          .setFields("id")
                                          .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return folderData.getId();
        }

        return null;
    }

    public File getFileMetadata(String fileId, String fields)
    {
        if(mDriveService != null)
        {
            File fileInfo;

            try
            {
                fileInfo = mDriveService.files()
                                        .get(fileId)
                                        .setFields(fields)
                                        .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return null;
            }

            return fileInfo;
        }

        return null;
    }

    public boolean moveFile(String fileId, String folderId)
    {
        if(mDriveService != null)
        {
            StringBuilder previousParents = new StringBuilder();
            File parentData, fileData;

            try
            {
                parentData = mDriveService.files()
                                          .get(fileId)
                                          .setFields("parents")
                                          .execute();
            }
            catch(IOException e)
            {
                Log.d(TAG, e.toString());
                return false;
            }

            for(String parentId : parentData.getParents())
            {
                previousParents.append(parentId);
                previousParents.append(',');
            }

            try
            {
                fileData = mDriveService.files()
                                        .update(fileId, null)
                                        .setAddParents(folderId)
                                        .setRemoveParents(previousParents.toString())
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

    public boolean deleteFile(String fileId)
    {
        if(mDriveService != null)
        {
            try
            {
                mDriveService.files()
                             .delete(fileId)
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

    public boolean downloadFile(String fileId, String localFilePath)
    {
        if(mDriveService != null)
        {
            try
            {
                Drive.Files.Get fileDownloadRequest = mDriveService.files().get(fileId);
                fileDownloadRequest.getMediaHttpDownloader().setProgressListener(new FileDownloadProgressListener());
                fileDownloadRequest.executeMediaAndDownloadTo(new FileOutputStream(localFilePath));
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

    public String uploadFile(String localFilePath, String name, String mimeType, String parentFolderId)
    {
        if(mDriveService != null)
        {
            java.io.File mediaFile = new java.io.File(localFilePath);
            File fileMetadata = new File();
            File uploadedFileData;

            fileMetadata.setName(name);
            if(!parentFolderId.isEmpty()) fileMetadata.setParents(Collections.singletonList(parentFolderId));

            try
            {
                Drive.Files.Create fileUploadRequest = mDriveService.files().create(fileMetadata, new FileContent(mimeType, mediaFile));
                fileUploadRequest.getMediaHttpUploader().setProgressListener(new FileUploadProgressListener());
                uploadedFileData = fileUploadRequest.setFields("id").execute();
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

            return uploadedFileData.getId();
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
