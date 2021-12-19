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
import android.database.Cursor;
import android.net.Uri;
import android.content.Intent;
import android.content.ContentResolver;
import android.provider.MediaStore;
import android.provider.MediaStore.MediaColumns;
import android.content.ContentValues;
import android.content.ContentResolver;
import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Build;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.IOException;

public class AndroidImages
{
    private final Activity mActivityInstance;

    public AndroidImages(Activity activityInstance)
    {
        mActivityInstance = activityInstance;
    }

    public AlbumInfo[] getAlbumsList()
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        AlbumInfo[] albumsList = null;
        Cursor cur;

        cur = resolver.query(MediaStore.Files.getContentUri("external"),
                             new String[]{ MediaStore.Files.FileColumns.PARENT, MediaStore.Images.Media.BUCKET_DISPLAY_NAME },
                             MediaStore.Files.FileColumns.MEDIA_TYPE + "=? ) GROUP BY ( " + MediaStore.Files.FileColumns.PARENT + " ",
                             new String[] { String.valueOf(MediaStore.Files.FileColumns.MEDIA_TYPE_IMAGE) },
                             null
                             );
        if(cur != null)
        {
            if(cur.moveToFirst())
            {
                final int idColumnIdx = cur.getColumnIndex(MediaStore.Files.FileColumns.PARENT);
                final int nameColumnIdx = cur.getColumnIndex(MediaStore.Images.Media.BUCKET_DISPLAY_NAME);
                albumsList = new AlbumInfo[cur.getCount()];

                for(int i = 0; i < albumsList.length; i++)
                {
                    AlbumInfo album = new AlbumInfo();

                    album.id = cur.getInt(idColumnIdx);
                    album.name = cur.getString(nameColumnIdx);
                    albumsList[i] = album;

                    cur.moveToNext();
                }
            }
            cur.close();
        }

        return albumsList;
    }

    public String[] getAlbumImagesList(int albumId)
    {
        final ContentResolver resolver = mActivityInstance.getContentResolver();
        String[] imagesList = null;
        Cursor cur;

        cur = resolver.query(MediaStore.Files.getContentUri("external"),
                             new String[]{ MediaStore.Images.Media.DATA },
                             MediaStore.Files.FileColumns.MEDIA_TYPE + "=? and " + MediaStore.Files.FileColumns.PARENT + "=?",
                             new String[] { String.valueOf(MediaStore.Files.FileColumns.MEDIA_TYPE_IMAGE), String.valueOf(albumId) },
                             " " + MediaStore.Images.Media.DATE_TAKEN + " DESC"
                             );
        if(cur != null)
        {
            if(cur.moveToFirst())
            {
                final int nameColumnIdx = cur.getColumnIndex(MediaStore.Images.Media.DATA);
                imagesList = new String[cur.getCount()];

                for(int i = 0; i < imagesList.length; i++)
                {
                    imagesList[i] = cur.getString(nameColumnIdx);
                    cur.moveToNext();
                }
            }
            cur.close();
        }

        return imagesList;
    }

    public void addImageToGallery(String imagePath)
    {
        final Intent mediaScanIntent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        final Uri contentUri = Uri.fromFile(new File(imagePath));

        mediaScanIntent.setData(contentUri);
        mActivityInstance.sendBroadcast(mediaScanIntent);
    }

    public boolean saveImageToGallery(String name, Bitmap bitmap)
    {
        final Intent mediaScanIntent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        OutputStream imageOutStream;
        Uri imageUri;

        try
        {
            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
            {
                final ContentResolver resolver = mActivityInstance.getContentResolver();
                final ContentValues values = new ContentValues();

                values.put(MediaStore.Images.Media.DISPLAY_NAME, name + ".jpg");
                values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
                values.put(MediaStore.Images.Media.RELATIVE_PATH, Environment.DIRECTORY_PICTURES);

                imageUri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
                imageOutStream = resolver.openOutputStream(imageUri);
            }
            else
            {
                final String imagesDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).toString();
                final File image = new File(imagesDir, name + ".jpg");

                imageUri = Uri.fromFile(image);
                imageOutStream = new FileOutputStream(image);
            }

            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, imageOutStream);
            imageOutStream.close();
        }
        catch(IOException e)
        {
            return false;
        }

        mediaScanIntent.setData(imageUri);
        mActivityInstance.sendBroadcast(mediaScanIntent);

        return true;
    }

    public boolean imageFileExist(String name)
    {
        final String imagesDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).toString();
        final File image = new File(imagesDir, name + ".jpg");

        return image.isFile();
    }

    public static class AlbumInfo
    {
        public int id = -1;
        public String name;
    }
}
