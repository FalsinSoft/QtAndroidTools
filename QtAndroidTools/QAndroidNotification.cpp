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
#include <QGuiApplication>
#include "QAndroidNotification.h"

QMap<int, QAndroidNotification*> QAndroidNotification::m_pInstancesMap;
int QAndroidNotification::m_InstancesCounter = 0;

QAndroidNotification::QAndroidNotification(QQuickItem *parent) : QQuickItem(parent),
                                                                 m_JavaNotification("com/falsinsoft/qtandroidtools/AndroidNotification",
                                                                                    "(Landroid/app/Activity;)V",
                                                                                    QtAndroid::androidActivity().object<jobject>()),
                                                                 m_InstanceIndex(m_InstancesCounter++)
{
    m_pInstancesMap[m_InstanceIndex] = this;
}

QAndroidNotification::~QAndroidNotification()
{
    m_pInstancesMap.remove(m_InstanceIndex);
}

const QMap<int, QAndroidNotification*>& QAndroidNotification::Instances()
{
    return m_pInstancesMap;
}
