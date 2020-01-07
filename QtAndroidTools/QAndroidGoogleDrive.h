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
#pragma once

#include <QtAndroidExtras>
#include <QQmlEngine>

class QAndroidGoogleDrive : public QObject
{
    Q_PROPERTY(QString SCOPE_DRIVE MEMBER SCOPE_DRIVE CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_APPDATA MEMBER SCOPE_DRIVE_APPDATA CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_FILE MEMBER SCOPE_DRIVE_FILE CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_METADATA MEMBER SCOPE_DRIVE_METADATA CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_METADATA_READONLY MEMBER SCOPE_DRIVE_METADATA_READONLY CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_PHOTOS_READONLY MEMBER SCOPE_DRIVE_PHOTOS_READONLY CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_READONLY MEMBER SCOPE_DRIVE_READONLY CONSTANT)
    Q_PROPERTY(QString SCOPE_DRIVE_SCRIPTS MEMBER SCOPE_DRIVE_SCRIPTS CONSTANT)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)
    Q_DISABLE_COPY(QAndroidGoogleDrive)
    Q_OBJECT

    QAndroidGoogleDrive();

public:
    const QString &SCOPE_DRIVE = m_ScopeList[0];
    const QString &SCOPE_DRIVE_APPDATA = m_ScopeList[1];
    const QString &SCOPE_DRIVE_FILE = m_ScopeList[2];
    const QString &SCOPE_DRIVE_METADATA = m_ScopeList[3];
    const QString &SCOPE_DRIVE_METADATA_READONLY = m_ScopeList[4];
    const QString &SCOPE_DRIVE_PHOTOS_READONLY = m_ScopeList[5];
    const QString &SCOPE_DRIVE_READONLY = m_ScopeList[6];
    const QString &SCOPE_DRIVE_SCRIPTS = m_ScopeList[7];

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidGoogleDrive* instance();

    Q_INVOKABLE bool authenticate(const QString &AppName, const QString &ScopeName);
    Q_INVOKABLE QVariantList getFilesList(const QString &Query = QString());
    Q_INVOKABLE QString getRootId();
    Q_INVOKABLE bool downloadFile(const QString &FileId, const QString &LocalFilePath);

    bool isAuthenticated() { return m_isAuthenticated; }

signals:
    void isAuthenticatedChanged();
    void downloadProgress(double progress);
    void downloadComplete();

private:
    const QAndroidJniObject m_JavaGoogleDrive;
    static QAndroidGoogleDrive *m_pInstance;
    bool m_isAuthenticated = false;
    QString m_ScopeList[8];

    static void DownloadProgress(JNIEnv *env, jobject thiz, jdouble Progress);
    static void DownloadComplete(JNIEnv *env, jobject thiz);

    void LoadScopeDefinitions();
};
