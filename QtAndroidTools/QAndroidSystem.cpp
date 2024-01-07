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
#include "QAndroidSystem.h"

QAndroidSystem *QAndroidSystem::m_pInstance = nullptr;

QAndroidSystem::QAndroidSystem(QObject *parent) : QObject(parent),
                                                  m_javaSystem("com/falsinsoft/qtandroidtools/AndroidSystem",
                                                  "(Landroid/app/Activity;)V",
                                                  QNativeInterface::QAndroidApplication::context())
{
    m_pInstance = this;
}

QAndroidSystem* QAndroidSystem::create(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidSystem();
}

QAndroidSystem* QAndroidSystem::instance()
{
    return m_pInstance;
}

int QAndroidSystem::spToPx(float sp)
{
    if(m_javaSystem.isValid())
    {
        return m_javaSystem.callMethod<jint>("spToPx", "(F)I", sp);
    }
    return -1;
}

int QAndroidSystem::dipToPx(float dip)
{
    if(m_javaSystem.isValid())
    {
        return m_javaSystem.callMethod<jint>("dipToPx", "(F)I", dip);
    }
    return -1;
}

int QAndroidSystem::ptToPx(float pt)
{
    if(m_javaSystem.isValid())
    {
        return m_javaSystem.callMethod<jint>("ptToPx", "(F)I", pt);
    }
    return -1;
}

bool QAndroidSystem::requestEmailSend(const QStringList &emailsList, const QString &subject, const QString &body, const QString &description)
{
    if(m_javaSystem.isValid())
    {
        const QJniObject stringObj("java/lang/String");
        QJniObject stringArrayObj;
        QJniEnvironment jniEnv;

        stringArrayObj = QJniObject::fromLocalRef(jniEnv->NewObjectArray(emailsList.count(), jniEnv->GetObjectClass(stringObj.object()), NULL));

        for(int i = 0; i < emailsList.count(); i++)
        {
            jniEnv->SetObjectArrayElement(stringArrayObj.object<jobjectArray>(), i, QJniObject::fromString(emailsList[i]).object<jstring>());
        }

        return m_javaSystem.callMethod<jboolean>("requestEmailSend",
                                                 "([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
                                                 stringArrayObj.object<jobjectArray>(),
                                                 QJniObject::fromString(subject).object<jstring>(),
                                                 QJniObject::fromString(body).object<jstring>(),
                                                 QJniObject::fromString(description).object<jstring>()
                                                 );
    }
    return false;
}
