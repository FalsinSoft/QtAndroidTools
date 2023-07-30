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

#include <QtCore/private/qandroidextras_p.h>
#include <QQmlEngine>
#include <QQuickItem>
#include <QVariant>

class QAndroidAppPermissions : public QQuickItem
{
	Q_OBJECT
    QML_NAMED_ELEMENT(QtAndroidAppPermissions)

    using PermissionResultMap = QHash<QString, QtAndroidPrivate::PermissionResult>;

public:
    QAndroidAppPermissions(QQuickItem *parent = nullptr);

    Q_INVOKABLE void requestPermissions(const QStringList &permissionsNameList);
    Q_INVOKABLE void requestPermission(const QString &permissionName);
    Q_INVOKABLE bool shouldShowRequestPermissionInfo(const QString &permissionName);
    Q_INVOKABLE bool isPermissionGranted(const QString &permissionName);

Q_SIGNALS:
    void requestPermissionsResults(const QVariantList &results);

private:
    QVariantList convertToVariantList(const PermissionResultMap &resultMap) const;
};
