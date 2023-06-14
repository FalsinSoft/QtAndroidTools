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
#include <QCoreApplication>
#include "QAndroidPlayStore.h"

QAndroidPlayStore *QAndroidPlayStore::m_pInstance = nullptr;

QAndroidPlayStore::QAndroidPlayStore(QObject *parent) : QObject(parent),
                                                        m_javaPlayStore("com/falsinsoft/qtandroidtools/AndroidPlayStore",
                                                                        "(Landroid/app/Activity;)V",
                                                                        QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;
}

QAndroidPlayStore* QAndroidPlayStore::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidPlayStore();
}

QAndroidPlayStore* QAndroidPlayStore::instance()
{
    return m_pInstance;
}

void QAndroidPlayStore::openAppDetails(const QString &packageName)
{
    QString detailsParam("details?id=");

    if(packageName.isEmpty())
    {
        const QJniObject activity = QNativeInterface::QAndroidApplication::context();
        detailsParam += activity.callObjectMethod("getPackageName", "()Ljava/lang/String;").toString();
    }
    else
    {
        detailsParam += packageName;
    }

    if(m_javaPlayStore.isValid())
    {
        m_javaPlayStore.callMethod<void>("open",
                                         "(Ljava/lang/String;)V",
                                         QJniObject::fromString(detailsParam).object<jstring>()
                                         );
    }
}

void QAndroidPlayStore::openDeveloperAppList(const QString &developerName)
{
    if(m_javaPlayStore.isValid())
    {
        m_javaPlayStore.callMethod<void>("open",
                                         "(Ljava/lang/String;)V",
                                         QJniObject::fromString("developer?id=" + developerName).object<jstring>()
                                         );
    }
}
