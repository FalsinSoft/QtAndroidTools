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
import android.os.Build;
import androidx.annotation.NonNull;
import androidx.core.content.FileProvider;
import android.content.ContentResolver;
import android.database.Cursor;
import android.provider.OpenableColumns;
import android.os.ParcelFileDescriptor;

import java.io.File;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
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

    @SuppressWarnings("deprecation")
    public byte[] getReceivedSharedBinaryData()
    {
        final Intent intent = mActivityInstance.getIntent();
        final Uri dataUri;

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU)
            dataUri = intent.getParcelableExtra(Intent.EXTRA_STREAM, Uri.class);
        else
            dataUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);

        if(dataUri != null)
        {
            return readAllBytesFromUri(dataUri);
        }

        return null;
    }

    @SuppressWarnings({"deprecation", "unchecked"})
    public byte[][] getReceivedMultipleSharedBinaryData()
    {
        final Intent intent = mActivityInstance.getIntent();
        final ArrayList<Uri> uriArray;

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU)
            uriArray = intent.getParcelableArrayListExtra(Intent.EXTRA_STREAM, Uri.class);
        else
            uriArray = intent.getParcelableArrayListExtra(Intent.EXTRA_STREAM);

        if (uriArray != null)
        {
            final int uriNum = uriArray.size();
            byte[][] multipleBinaryData = new byte[uriNum][];

            for(int i = 0; i < uriNum; i++)
            {
                byte[] fileData = readAllBytesFromUri(uriArray.get(i));
                if(fileData == null) return null;
                multipleBinaryData[i] = fileData;
            }

            return multipleBinaryData;
        }

        return null;
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
        final Intent sendIntent = new Intent();
        Uri fileUri;

        try
        {
            fileUri = FileProvider.getUriForFile(mActivityInstance,
                                                 packageName + ".qtprovider",
                                                 new File(dataFilePath)
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
        final Intent returnFileIntent = new Intent(packageName + ".ACTION_RETURN_FILE");

        if(fileAvailable == true)
        {
            Uri fileUri;

            try
            {
                fileUri = FileProvider.getUriForFile(mActivityInstance,
                                                     packageName + ".qtprovider",
                                                     new File(filePath)
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
                FileInputStream dataStream = new FileInputStream(mRequestedSharedFile.getFileDescriptor());
                ByteArrayOutputStream buffer = new ByteArrayOutputStream();
                byte[] data = new byte[8192];
                int nRead;

                while((nRead = dataStream.read(data, 0, data.length)) != -1)
                {
                    buffer.write(data, 0, nRead);
                }
                buffer.flush();
                fileData = buffer.toByteArray();

            }
            catch(IOException e)
            {
                Log.e(TAG, "Error reading requested shared file", e);
                return null;
            }
            finally
            {
                closeRequestedSharedFile();
            }
        }

        return fileData;
    }

    public Intent getRequestSharedFileIntent(String mimeType)
    {
        final Intent requestFileIntent = new Intent(Intent.ACTION_GET_CONTENT);
        requestFileIntent.addCategory(Intent.CATEGORY_OPENABLE);
        requestFileIntent.setType(mimeType);
        return requestFileIntent;
    }

    public boolean requestSharedFileIntentDataResult(Intent data)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        final Uri sharedFileUri = data.getData();
        String fileName = "unknown";
        String mimeType;
        long fileSize = -1;

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

        try
        {
            final Cursor dataCursor = resolver.query(sharedFileUri, null, null, null, null);

            if(dataCursor != null && dataCursor.moveToFirst())
            {
                final int nameIndex = dataCursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                final int sizeIndex = dataCursor.getColumnIndex(OpenableColumns.SIZE);

                if(nameIndex != -1) fileName = dataCursor.getString(nameIndex);
                if(sizeIndex != -1) fileSize = dataCursor.getLong(sizeIndex);
            }
        }
        catch(Exception e)
        {
            Log.e(TAG, "Error querying file details", e);
        }

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

    private byte[] readAllBytesFromUri(@NonNull Uri uri)
    {
        try
        {
            final InputStream inputStream = mActivityInstance.getContentResolver().openInputStream(uri);

            if(inputStream != null)
            {
                final ByteArrayOutputStream buffer = new ByteArrayOutputStream();
                byte[] data = new byte[8192];
                int nRead;

                while((nRead = inputStream.read(data, 0, data.length)) != -1)
                {
                    buffer.write(data, 0, nRead);
                }
                buffer.flush();
                return buffer.toByteArray();
            }
        }
        catch(IOException e)
        {
            Log.e(TAG, "Failed to read data from Uri: " + uri, e);
        }

        return null;
    }

    private static native void requestedSharedFileInfo(String mimeType, String name, long size);
}
