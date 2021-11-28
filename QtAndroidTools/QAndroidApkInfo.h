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

class QAndroidApkInfo : public QObject
{
    Q_PROPERTY(qlonglong firstInstallTime READ getFirstInstallTime CONSTANT)
    Q_PROPERTY(qlonglong lastUpdateTime READ getLastUpdateTime CONSTANT)
    Q_PROPERTY(QString packageName READ getPackageName CONSTANT)
    Q_PROPERTY(QStringList requestedPermissions READ getRequestedPermissions CONSTANT)
    Q_PROPERTY(int versionCode READ getVersionCode CONSTANT)
    Q_PROPERTY(QString versionName READ getVersionName CONSTANT)
	Q_DISABLE_COPY(QAndroidApkInfo)
    Q_OBJECT

    QAndroidApkInfo() : QAndroidApkInfo(nullptr) {}

public:
    QAndroidApkInfo(QObject *parent);

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QAndroidApkInfo* instance();

    qlonglong getFirstInstallTime() const;
    qlonglong getLastUpdateTime() const;
    const QString& getPackageName() const;
    const QStringList& getRequestedPermissions() const;
    int getVersionCode() const;
    const QString& getVersionName() const;

private:
    static QAndroidApkInfo *m_pInstance;

    struct {
        qlonglong firstInstallTime;
        qlonglong lastUpdateTime;
        QString packageName;
        QStringList requestedPermissions;
        int versionCode;
        QString versionName;
    } m_apkInfo;

    void loadApkPackageInfo();
    QStringList getStringListField(const QAndroidJniObject &jniObject, const QString &fieldName) const;
};
