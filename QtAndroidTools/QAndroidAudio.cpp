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
#include "QAndroidAudio.h"

QAndroidAudio *QAndroidAudio::m_pInstance = nullptr;

QAndroidAudio::QAndroidAudio(QObject *parent) : QObject(parent),
                                                m_javaAudio("com/falsinsoft/qtandroidtools/AndroidAudio",
                                                            "(Landroid/app/Activity;)V",
                                                            QtAndroid::androidActivity().object<jobject>()),
                                                m_focus(false)
{
    m_pInstance = this;

    if(m_javaAudio.isValid())
    {
        const JNINativeMethod jniMethod[] = {
            {"focusChanged", "(Z)V", reinterpret_cast<void*>(&QAndroidAudio::deviceFocusChanged)},
        };
        QAndroidJniEnvironment jniEnv;
        jclass objectClass;

        objectClass = jniEnv->GetObjectClass(m_javaAudio.object<jobject>());
        jniEnv->RegisterNatives(objectClass, jniMethod, sizeof(jniMethod)/sizeof(JNINativeMethod));
        jniEnv->DeleteLocalRef(objectClass);
    }
}

QAndroidAudio::~QAndroidAudio()
{
}

QObject* QAndroidAudio::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidAudio();
}

QAndroidAudio* QAndroidAudio::instance()
{
    return m_pInstance;
}

void QAndroidAudio::deviceFocusChanged(JNIEnv *env, jobject thiz, jboolean focus)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if(m_pInstance != nullptr)
    {
        Q_EMIT m_pInstance->setFocus(focus);
    }
}

bool QAndroidAudio::hasFocus()
{
    return m_focus;
}

void QAndroidAudio::setFocus(bool focus)
{
    m_focus = focus;
    Q_EMIT focusChanged();
}

bool QAndroidAudio::requestFocus()
{
    if(m_javaAudio.isValid())
    {
        if(m_javaAudio.callMethod<jboolean>("requestFocus"))
        {
            setFocus(true);
            return true;
        }
    }
    return false;
}

void QAndroidAudio::abandonFocus()
{
    if(m_javaAudio.isValid())
    {
        m_javaAudio.callMethod<void>("abandonFocus");
        setFocus(false);
    }
}
