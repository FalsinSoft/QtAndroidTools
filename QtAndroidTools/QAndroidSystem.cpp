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
#include "QAndroidSystem.h"

QAndroidSystem *QAndroidSystem::m_pInstance = nullptr;

QAndroidSystem::QAndroidSystem()
{
    m_pInstance = this;
    loadStandardPaths();
}

QObject* QAndroidSystem::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QAndroidSystem();
}

QAndroidSystem* QAndroidSystem::instance()
{
    return m_pInstance;
}

const QString& QAndroidSystem::getDataLocation() const
{
    return m_standardPaths.dataLocation;
}

const QString& QAndroidSystem::getConfigLocation() const
{
    return m_standardPaths.configLocation;
}

const QString& QAndroidSystem::getDownloadLocation() const
{
    return m_standardPaths.downloadLocation;
}

const QString& QAndroidSystem::getMusicLocation() const
{
    return m_standardPaths.musicLocation;
}

const QString& QAndroidSystem::getMoviesLocation() const
{
    return m_standardPaths.moviesLocation;
}

const QString& QAndroidSystem::getPicturesLocation() const
{
    return m_standardPaths.picturesLocation;
}

void QAndroidSystem::loadStandardPaths()
{
    m_standardPaths.dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    m_standardPaths.configLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    m_standardPaths.downloadLocation = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    m_standardPaths.musicLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    m_standardPaths.moviesLocation = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    m_standardPaths.picturesLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}
