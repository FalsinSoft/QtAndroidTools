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
import android.os.Bundle;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import androidx.core.content.FileProvider;
import android.content.ContentResolver;
import android.database.Cursor;
import android.provider.OpenableColumns;
import android.os.ParcelFileDescriptor;

import java.io.File;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileDescriptor;
import java.util.ArrayList;

public class AndroidSharing
{
    private static final String TAG = "AndroidSharing";
    private final Activity mActivityInstance;
    private ParcelFileDescriptor mRequestedSharedFile = null;

    public AndroidSharing(Context context)
    {
        mActivityInstance = (Activity)context;
    }

    public String getReceivedSharedText()
    {
        return mActivityInstance.getIntent().getStringExtra(Intent.EXTRA_TEXT);
    }

    public byte[] getReceivedSharedBinaryData()
    {
        final Uri dataUri = (Uri) mActivityInstance.getIntent().getParcelableExtra(Intent.EXTRA_STREAM);
        byte[] binaryData = null;

        if(dataUri != null)
        {
            try
            {
                final InputStream dataStream = mActivityInstance.getContentResolver().openInputStream(dataUri);
                binaryData = new byte[dataStream.available()];
                dataStream.read(binaryData);
            }
            catch(FileNotFoundException e)
            {
                return null;
            }
            catch(IOException e)
            {
                return null;
            }
        }

        return binaryData;
    }

    public byte[][] getReceivedMultipleSharedBinaryData()
    {
        final ArrayList<Uri> uriArray = mActivityInstance.getIntent().getParcelableArrayListExtra(Intent.EXTRA_STREAM);
        byte[][] multipleBinaryData = null;

        if(uriArray != null)
        {
            final int uriNum = uriArray.size();

            multipleBinaryData = new byte[uriNum][];

            for(int i = 0; i < uriNum; i++)
            {
                try
                {
                    final InputStream dataStream = mActivityInstance.getContentResolver().openInputStream(uriArray.get(i));
                    multipleBinaryData[i] = new byte[dataStream.available()];
                    dataStream.read(multipleBinaryData[i]);
                }
                catch(FileNotFoundException e)
                {
                    return null;
                }
                catch(IOException e)
                {
                    return null;
                }
            }
        }

        return multipleBinaryData;
    }

    public boolean shareText(String text)
    {
        final Intent sendIntent = new Intent();

        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_TEXT, text);
        sendIntent.setType("text/plain");

        mActivityInstance.startActivity(Intent.createChooser(sendIntent, null));
        return true;
    }

    public boolean shareBinaryData(String mimeType, String dataFilePath)
    {
        final String packageName = mActivityInstance.getApplicationContext().getPackageName();
        final String filesDir = (mActivityInstance.getFilesDir().getAbsolutePath() + "/");
        final Intent sendIntent = new Intent();
        Uri fileUri;

        try
        {
            fileUri = FileProvider.getUriForFile(mActivityInstance,
                                                 packageName + ".qtprovider",
                                                 new File(filesDir + dataFilePath)
                                                 );
        }
        catch(IllegalArgumentException e)
        {
            Log.e(TAG, "The selected file can't be shared: " + dataFilePath);
            return false;
        }

        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_STREAM, fileUri);
        sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        sendIntent.setType(mimeType);

        mActivityInstance.startActivity(Intent.createChooser(sendIntent, null));
        return true;
    }

    public boolean shareFile(boolean fileAvailable, String mimeType, String filePath)
    {
        final String packageName = mActivityInstance.getApplicationContext().getPackageName();
        final String filesDir = (mActivityInstance.getFilesDir().getAbsolutePath() + "/");
        final Intent returnFileIntent = new Intent(packageName + ".ACTION_RETURN_FILE");

        if(fileAvailable == true)
        {
            Uri fileUri;

            try
            {
                fileUri = FileProvider.getUriForFile(mActivityInstance,
                                                     packageName + ".qtprovider",
                                                     new File(filesDir + filePath)
                                                     );
            }
            catch(IllegalArgumentException e)
            {
                Log.e(TAG, "The selected file can't be shared: " + filePath);
                return false;
            }

            returnFileIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            returnFileIntent.setDataAndType(fileUri, mimeType);
            mActivityInstance.setResult(Activity.RESULT_OK, returnFileIntent);
        }
        else
        {
            returnFileIntent.setDataAndType(null, "");
            mActivityInstance.setResult(Activity.RESULT_CANCELED, returnFileIntent);
        }

        return true;
    }

    public byte[] getRequestedSharedFile()
    {
        byte[] fileData = null;

        if(mRequestedSharedFile != null)
        {
            try
            {
                final FileInputStream dataStream = new FileInputStream(mRequestedSharedFile.getFileDescriptor());
                fileData = new byte[dataStream.available()];
                dataStream.read(fileData);
            }
            catch(IOException e)
            {
                return null;
            }

            closeRequestedSharedFile();
        }

        return fileData;
    }

    public Intent getRequestSharedFileIntent(String mimeType)
    {
        final Intent requestFileIntent = new Intent(Intent.ACTION_PICK);
        requestFileIntent.setType(mimeType);
        return requestFileIntent;
    }

    public boolean requestSharedFileIntentDataResult(Intent data)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        final Uri sharedFileUri = data.getData();
        String fileName, mimeType;
        Cursor dataCursor;
        long fileSize;

        closeRequestedSharedFile();

        try
        {
            mRequestedSharedFile = resolver.openFileDescriptor(sharedFileUri, "r");
        }
        catch(FileNotFoundException e)
        {
            Log.e(TAG, "Shared file not found");
            return false;
        }

        dataCursor = resolver.query(sharedFileUri, null, null, null, null);
        dataCursor.moveToFirst();
        fileName = dataCursor.getString(dataCursor.getColumnIndex(OpenableColumns.DISPLAY_NAME));
        fileSize = dataCursor.getLong(dataCursor.getColumnIndex(OpenableColumns.SIZE));
        mimeType = resolver.getType(sharedFileUri);

        requestedSharedFileInfo(mimeType, fileName, fileSize);
        return true;
    }

    public void closeRequestedSharedFile()
    {
        if(mRequestedSharedFile != null)
        {
            try
            {
                mRequestedSharedFile.close();
            }
            catch(IOException e)
            {
            }
            mRequestedSharedFile = null;
        }
    }

    private static native void requestedSharedFileInfo(String mimeType, String name, long size);
}
