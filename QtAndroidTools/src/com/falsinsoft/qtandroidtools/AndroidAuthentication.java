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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.DialogInterface;
import android.provider.Settings;
import android.os.Build;
import android.util.Log;
import android.util.Base64;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.hardware.biometrics.BiometricManager;
import android.hardware.biometrics.BiometricPrompt;
import android.security.keystore.KeyGenParameterSpec;
import android.security.keystore.KeyProperties;

import java.nio.charset.StandardCharsets;
import java.util.concurrent.Executor;
import java.security.KeyStore;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.GCMParameterSpec;

public class AndroidAuthentication
{
    private static final String TAG = "AndroidAuthentication";
    private final Activity mActivityInstance;

    private int mAuthenticators = 0;
    private String mTitle = new String();
    private String mDescription = new String();
    private String mNegativeButton = new String();
    private CancellationSignal mCancellationSignal = new CancellationSignal();

    private enum CryptoTask
    {
        NONE,
        ENCRYPT,
        DECRYPT
    }

    public AndroidAuthentication(Context context)
    {
        mActivityInstance = (Activity)context;
        mCancellationSignal.setOnCancelListener(new CancellationSignalListener());
    }

    public void setAuthenticators(int authenticators)
    {
        mAuthenticators = 0;
        if((authenticators & BIOMETRIC_STRONG) == BIOMETRIC_STRONG) mAuthenticators |= BiometricManager.Authenticators.BIOMETRIC_STRONG;
        if((authenticators & BIOMETRIC_WEAK) == BIOMETRIC_WEAK) mAuthenticators |= BiometricManager.Authenticators.BIOMETRIC_WEAK;
        if((authenticators & DEVICE_CREDENTIAL) == DEVICE_CREDENTIAL) mAuthenticators |= BiometricManager.Authenticators.DEVICE_CREDENTIAL;
    }

    public void setTitle(String title)
    {
        mTitle = title;
    }

    public void setDescription(String description)
    {
        mDescription = description;
    }

    public void setNegativeButton(String negativeButton)
    {
        mNegativeButton = negativeButton;
    }

    public void cancel()
    {
        mCancellationSignal.cancel();
    }

    public int canAuthenticate()
    {
        final BiometricManager biometricManager = mActivityInstance.getSystemService(BiometricManager.class);
        int result = BIOMETRIC_STATUS_UNKNOWN;

        switch(biometricManager.canAuthenticate(mAuthenticators))
        {
            case BiometricManager.BIOMETRIC_SUCCESS:
                result = BIOMETRIC_SUCCESS;
                break;
            case BiometricManager.BIOMETRIC_ERROR_NO_HARDWARE:
                result = BIOMETRIC_ERROR_NO_HARDWARE;
                break;
            case BiometricManager.BIOMETRIC_ERROR_HW_UNAVAILABLE:
                result = BIOMETRIC_ERROR_HW_UNAVAILABLE;
                break;
            case BiometricManager.BIOMETRIC_ERROR_NONE_ENROLLED:
                result = BIOMETRIC_ERROR_NONE_ENROLLED;
                break;
            case BiometricManager.BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED:
                result = BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED;
                break;
        }

        return result;
    }

    public boolean requestBiometricEnroll()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
        {
            final Intent enrollIntent = new Intent(Settings.ACTION_BIOMETRIC_ENROLL);
            enrollIntent.putExtra(Settings.EXTRA_BIOMETRIC_AUTHENTICATORS_ALLOWED, mAuthenticators);
            mActivityInstance.startActivityForResult(enrollIntent, 123456);
            return true;
        }

        Log.d(TAG, "Could not request biometric enrollment in API level < 30");
        return false;
    }

    public boolean authenticate()
    {
        final BiometricPrompt prompt = buildPrompt();

        if(prompt != null)
        {
            prompt.authenticate(mCancellationSignal,
                                mActivityInstance.getMainExecutor(),
                                new BiometricAuthenticationCallback(CryptoTask.NONE, null));
            return true;
        }

        return false;
    }

    public boolean authenticateAndEncrypt(String plainText)
    {
        final BiometricPrompt prompt = buildPrompt();

        if(prompt != null)
        {
            try
            {
                prompt.authenticate(new BiometricPrompt.CryptoObject(CryptoConfig.newCipherForEncrypt()),
                                    mCancellationSignal,
                                    mActivityInstance.getMainExecutor(),
                                    new BiometricAuthenticationCallback(CryptoTask.ENCRYPT, plainText.getBytes(StandardCharsets.UTF_8)));
                return true;
            }
            catch(Exception e)
            {
                Log.d(TAG, "Init encrypt cipher failed: " + e.getMessage());
            }
        }

        return false;
    }

    public boolean authenticateAndDecrypt(String encryptedText, String initializationVector)
    {
        final BiometricPrompt prompt = buildPrompt();

        if(prompt != null)
        {
            try
            {
                prompt.authenticate(new BiometricPrompt.CryptoObject(CryptoConfig.newCipherForDecrypt(Base64.decode(initializationVector, Base64.NO_WRAP))),
                                    mCancellationSignal,
                                    mActivityInstance.getMainExecutor(),
                                    new BiometricAuthenticationCallback(CryptoTask.DECRYPT, Base64.decode(encryptedText, Base64.NO_WRAP)));
                return true;
            }
            catch(Exception e)
            {
                Log.d(TAG, "Init decrypt cipher failed: " + e.getMessage());
            }
        }

        return false;
    }

    private BiometricPrompt buildPrompt()
    {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            final BiometricPrompt.Builder builder = new BiometricPrompt.Builder(mActivityInstance);

            builder.setTitle(mTitle);
            builder.setDescription(mDescription);
            builder.setConfirmationRequired(false);
            builder.setAllowedAuthenticators(mAuthenticators);
            builder.setNegativeButton(mNegativeButton, mActivityInstance.getMainExecutor(), new NegativeButtonListener());

            return builder.build();
        }

        Log.d(TAG, "Could not request biometric authentication in API level < 28");
        return null;
    }

    private class BiometricAuthenticationCallback extends BiometricPrompt.AuthenticationCallback
    {
        private final CryptoTask mCryptoTask;
        private final byte[] mCipherText;

        public BiometricAuthenticationCallback(CryptoTask cryptoTask, byte[] cipherText)
        {
            mCryptoTask = cryptoTask;
            mCipherText = cipherText;
        }

        @Override
        public void onAuthenticationError(int errorCode, CharSequence errString)
        {
            super.onAuthenticationError(errorCode, errString);
            authenticationError(errString.toString());
        }

        @Override
        public void onAuthenticationSucceeded(BiometricPrompt.AuthenticationResult result)
        {
            super.onAuthenticationSucceeded(result);

            if(mCryptoTask == CryptoTask.ENCRYPT || mCryptoTask == CryptoTask.DECRYPT)
            {
                try
                {
                    BiometricPrompt.CryptoObject cryptoObj;
                    Cipher cipher;

                    cryptoObj = result.getCryptoObject();
                    if(cryptoObj == null)
                    {
                        authenticationError("No crypto object available");
                        return;
                    }
                    cipher = cryptoObj.getCipher();
                    if(cipher == null)
                    {
                        authenticationError("No crypto cipher available");
                        return;
                    }

                    if(mCryptoTask == CryptoTask.ENCRYPT)
                        authenticationAndEncryptionSucceeded(Base64.encodeToString(cipher.doFinal(mCipherText), Base64.NO_WRAP), Base64.encodeToString(cipher.getIV(), Base64.NO_WRAP));
                    else
                        authenticationAndDecryptionSucceeded(new String(cipher.doFinal(mCipherText), StandardCharsets.UTF_8));
                }
                catch(Exception e)
                {
                    authenticationError("Enecrypt/decrypt failed: " + e.getMessage());
                    return;
                }
            }
            else
            {
                authenticationSucceeded();
            }
        }

        @Override
        public void onAuthenticationFailed()
        {
            super.onAuthenticationFailed();
            authenticationFailed();
        }
    }

    private class CancellationSignalListener implements CancellationSignal.OnCancelListener
    {
        @Override
        public void onCancel()
        {
            authenticationCancelled();
        }
    }

    private class NegativeButtonListener implements DialogInterface.OnClickListener
    {
        @Override
        public void onClick(DialogInterface dialog, int which)
        {
            authenticationCancelled();
        }
    }

    private static final class CryptoConfig
    {
        private static final String KEY_ALIAS = "qtandroidtools_biometric_aes_key";
        private static final int KEY_SIZE_BITS = 256;
        private static final String BLOCK_MODE = KeyProperties.BLOCK_MODE_GCM;
        private static final String PADDING = KeyProperties.ENCRYPTION_PADDING_NONE;
        private static final String ALGORITHM = KeyProperties.KEY_ALGORITHM_AES;

        public static SecretKey getOrCreateSecretKey() throws Exception
        {
            final KeyStore keyStore = KeyStore.getInstance("AndroidKeyStore");
            KeyGenParameterSpec.Builder builder;
            KeyStore.Entry existing;
            KeyGenerator keyGen;

            keyStore.load(null);
            existing = keyStore.getEntry(KEY_ALIAS, null);
            if(existing instanceof KeyStore.SecretKeyEntry)
            {
                return ((KeyStore.SecretKeyEntry) existing).getSecretKey();
            }

            builder = new KeyGenParameterSpec.Builder(KEY_ALIAS, KeyProperties.PURPOSE_ENCRYPT | KeyProperties.PURPOSE_DECRYPT);
            builder.setBlockModes(BLOCK_MODE);
            builder.setEncryptionPaddings(PADDING);
            builder.setKeySize(KEY_SIZE_BITS);
            builder.setUserAuthenticationRequired(true);
            builder.setInvalidatedByBiometricEnrollment(true);

            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
                builder.setUserAuthenticationParameters(0, KeyProperties.AUTH_BIOMETRIC_STRONG);
            else
                builder.setUserAuthenticationValidityDurationSeconds(-1);

            keyGen = KeyGenerator.getInstance(ALGORITHM, "AndroidKeyStore");
            keyGen.init(builder.build());
            return keyGen.generateKey();
        }

        public static Cipher newCipherForEncrypt() throws Exception
        {
            final Cipher cipher = Cipher.getInstance(ALGORITHM + "/" + BLOCK_MODE + "/" + PADDING);
            final SecretKey key = getOrCreateSecretKey();
            cipher.init(Cipher.ENCRYPT_MODE, key);
            return cipher;
        }

        public static Cipher newCipherForDecrypt(byte[] iv) throws Exception
        {
            final Cipher cipher = Cipher.getInstance(ALGORITHM + "/" + BLOCK_MODE + "/" + PADDING);
            final SecretKey key = getOrCreateSecretKey();
            GCMParameterSpec spec = new GCMParameterSpec(128, iv);
            cipher.init(Cipher.DECRYPT_MODE, key, spec);
            return cipher;
        }
    }

    private final int BIOMETRIC_STRONG = 0x01;
    private final int BIOMETRIC_WEAK = 0x02;
    private final int DEVICE_CREDENTIAL = 0x04;

    private final int BIOMETRIC_STATUS_UNKNOWN = 0;
    private final int BIOMETRIC_SUCCESS = 1;
    private final int BIOMETRIC_ERROR_NO_HARDWARE = 2;
    private final int BIOMETRIC_ERROR_HW_UNAVAILABLE = 3;
    private final int BIOMETRIC_ERROR_NONE_ENROLLED = 4;
    private final int BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED = 5;

    private static native void authenticationError(String error);
    private static native void authenticationSucceeded();
    private static native void authenticationAndEncryptionSucceeded(String encryptedText, String initializationVector);
    private static native void authenticationAndDecryptionSucceeded(String decryptedText);
    private static native void authenticationFailed();
    private static native void authenticationCancelled();
}
