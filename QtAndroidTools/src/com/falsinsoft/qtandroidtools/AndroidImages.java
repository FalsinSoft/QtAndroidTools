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
import android.content.ContentUris;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;
import android.content.ContentResolver;
import android.provider.MediaStore;
import android.content.ContentValues;
import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Build;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.ArrayList;

public class AndroidImages
{
    private static final String TAG = "AndroidImages";
    private static final int FORMAT_JPEG = 0;
    private static final int FORMAT_PNG = 1;
    private final Activity mActivityInstance;

    public AndroidImages(Context context)
    {
        mActivityInstance = (Activity)context;
    }

    public String[] getAlbumsList()
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        final String[] projection = new String[]{ MediaStore.Images.Media.BUCKET_DISPLAY_NAME };
        String[] albumsList = null;

        try(Cursor cur = resolver.query(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                projection,
                null,
                null,
                MediaStore.Images.Media.BUCKET_DISPLAY_NAME + " ASC"))
        {
            if(cur != null && cur.getCount() > 0)
            {
                final int nameColumnIdx = cur.getColumnIndexOrThrow(MediaStore.Images.Media.BUCKET_DISPLAY_NAME);
                ArrayList<String> albums = new ArrayList<String>();

                while(cur.moveToNext())
                {
                    final String name = cur.getString(nameColumnIdx);
                    if(name != null && !albums.contains(name))
                    {
                        albums.add(name);
                        Log.d(TAG, name);
                    }
                }

                albumsList = new String[albums.size()];
                albumsList = albums.toArray(albumsList);
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        return albumsList;
    }

    public String[] getAlbumImagesList(String albumName)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        final String[] projection = new String[]{ MediaStore.Images.Media._ID };
        final String selection = (MediaStore.Images.Media.BUCKET_DISPLAY_NAME + "=?");
        final String sortOrder = (MediaStore.Images.Media.DATE_TAKEN + " DESC");
        final String[] selectionArgs = new String[]{ albumName };
        String[] imagesList = null;

        try(Cursor cur = resolver.query(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                projection,
                selection,
                selectionArgs,
                sortOrder))
        {
            if(cur != null && cur.getCount() > 0)
            {
                final int idColumnIdx = cur.getColumnIndexOrThrow(MediaStore.Images.Media._ID);
                ArrayList<String> images = new ArrayList<String>();

                while(cur.moveToNext())
                {
                    final Uri imageUri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, cur.getLong(idColumnIdx));
                    images.add(imageUri.toString());
                }

                imagesList = new String[images.size()];
                imagesList = images.toArray(imagesList);
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        return imagesList;
    }

    public byte[] getImageFromUri(String uri)
    {
        try
        {
            final ContentResolver resolver = mActivityInstance.getContentResolver();
            final Uri imageUri = Uri.parse(uri);

            try(InputStream inputStream = resolver.openInputStream(imageUri);
                ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream())
            {
                if(inputStream != null)
                {
                    byte[] buffer = new byte[8192];
                    int bytesRead;

                    while((bytesRead = inputStream.read(buffer)) != -1)
                    {
                        byteArrayOutputStream.write(buffer, 0, bytesRead);
                    }
                    return byteArrayOutputStream.toByteArray();
                }
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        return null;
    }

    public boolean saveImageToGallery(String name, Bitmap bitmap, int format)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        final ContentValues values = new ContentValues();
        Bitmap.CompressFormat compressFormat;
        String fileExtension;
        String mimeType;
        Uri imageUri;

        switch(format)
        {
            case FORMAT_JPEG:
                compressFormat = Bitmap.CompressFormat.JPEG;
                fileExtension = "jpg";
                mimeType = "image/jpeg";
                break;
            case FORMAT_PNG:
                compressFormat = Bitmap.CompressFormat.PNG;
                fileExtension = "png";
                mimeType = "image/png";
                break;
            default:
                return false;
        }

        values.put(MediaStore.Images.Media.DISPLAY_NAME, name + "." + fileExtension);
        values.put(MediaStore.Images.Media.MIME_TYPE, mimeType);

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
        {
            values.put(MediaStore.Images.Media.RELATIVE_PATH, Environment.DIRECTORY_PICTURES);
            values.put(MediaStore.Images.Media.IS_PENDING, 1);
        }

        imageUri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
        if(imageUri == null) return false;

        try(OutputStream imageOutStream = resolver.openOutputStream(imageUri))
        {
            if(imageOutStream == null)
            {
                return false;
            }
            bitmap.compress(compressFormat, 100, imageOutStream);
        }
        catch(IOException e)
        {
            e.printStackTrace();
            return false;
        }

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
        {
            values.clear();
            values.put(MediaStore.Images.Media.IS_PENDING, 0);
            resolver.update(imageUri, values, null, null);
        }

        return true;
    }

    public boolean imageFileExist(String name)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        String selection = MediaStore.Images.Media.DISPLAY_NAME + " = ?";
        String[] selectionArgs = new String[]{name};

        try(Cursor cur = resolver.query(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                new String[]{MediaStore.Images.Media._ID},
                selection,
                selectionArgs,
                null))
        {
            return (cur != null && cur.getCount() > 0) ? true : false;
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return false;
        }
    }
}