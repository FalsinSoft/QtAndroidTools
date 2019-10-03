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
#include "QAndroidGoogleDrive.h"

QAndroidGoogleDrive *QAndroidGoogleDrive::m_pInstance = nullptr;

QAndroidGoogleDrive::QAndroidGoogleDrive() : m_JavaGoogleDrive("com/falsinsoft/qtandroidtools/AndroidGoogleDrive",
                                                               "(Landroid/app/Activity;)V",
                                                               QtAndroid::androidActivity().object<jobject>())
{
    m_pInstance = this;
    LoadScopeDefinitions();
}

QObject* QAndroidGoogleDrive::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidGoogleDrive();
}

QAndroidGoogleDrive* QAndroidGoogleDrive::instance()
{
    return m_pInstance;
}

void QAndroidGoogleDrive::LoadScopeDefinitions()
{
    const char ScopesClass[] = "com/google/api/services/drive/DriveScopes";

    m_ScopeList[0] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE").toString();
    m_ScopeList[1] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_APPDATA").toString();
    m_ScopeList[2] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_FILE").toString();
    m_ScopeList[3] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_METADATA").toString();
    m_ScopeList[4] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_METADATA_READONLY").toString();
    m_ScopeList[5] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_PHOTOS_READONLY").toString();
    m_ScopeList[6] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_READONLY").toString();
    m_ScopeList[7] = QAndroidJniObject::getStaticObjectField<jstring>(ScopesClass, "DRIVE_SCRIPTS").toString();
}
