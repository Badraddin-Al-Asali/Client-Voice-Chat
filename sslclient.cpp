/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "sslclient.h"
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocket>
#include <QCoreApplication>

QT_USE_NAMESPACE

//! [constructor]
SslClient::SslClient(const QUrl &url, const double& MicTheshold, AudioEncoder *AudioEncoderPtr, QObject *parent) :
    QObject(parent),
    myNextBuffer(0),
    m_Inputdevice(QAudioDeviceInfo::defaultInputDevice()),
    m_Outputdevice(QAudioDeviceInfo::defaultOutputDevice()),
    m_audioInput(0),
    m_audioOutput(0),
    m_input(0),
    headerImage(0),
    headerAudio(1),
    MicTheshold(MicTheshold),
    AudioEncoderPtr(AudioEncoderPtr)
{
    for(int i(0); i < MaxBufferQUEUE; ++i)
        m_buffer[i] = QByteArray(BufferSize, 0);
    //qDebug() << "AA";
    connect(&m_webSocket, &QWebSocket::connected, this, &SslClient::onConnected);
    //qDebug() << "AB";
    connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
            this, &SslClient::onSslErrors);

    //qDebug() << "AC";
    connect(&m_webSocket, &QWebSocket::disconnected, this, &SslClient::onDisconnected);


    initializeAudio();
    //Audio output device
    m_output = m_audioOutput->start();
     //Audio input device
    m_input = m_audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(m_input, &QIODevice::readyRead, this, &SslClient::readMore);
    //qDebug() << "AD";
    //qDebug() << url;
    m_webSocket.open(QUrl(url));
    //qDebug() << "AE";
}

//Initialize audio
void SslClient::initializeAudio()
{

    m_format.setSampleRate(44100); //set frequency to 44100
    m_format.setChannelCount(1); //set channels to mono
    m_format.setSampleSize(16); //set sample sze to 16 bit
    m_format.setSampleType(QAudioFormat::UnSignedInt); //Sample type as usigned integer sample
    m_format.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    m_format.setCodec("audio/pcm"); //set codec as simple audio/pcm

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format))
    {
        //Default format not supported - trying to use nearest
        m_format = infoIn.nearestFormat(m_format);
    }

    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());

    if (!infoOut.isFormatSupported(m_format))
    {
       //Default format not supported - trying to use nearest
        m_format = infoOut.nearestFormat(m_format);
    }
    createAudioInput();
    createAudioOutput();
    //qDebug() << "TEST";
}

void SslClient::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_Outputdevice, m_format, this);
}
void SslClient::createAudioInput()
{
    if (m_input != 0) {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioInput = new QAudioInput(m_Inputdevice, m_format, this);

}
void SslClient::readMore()
{
    //Return if audio input is null
    if(!m_audioInput)
        return;

    //Check the number of samples in input buffer
    qint64 len = m_audioInput->bytesReady();

    //Limit sample size
    if(len > 4096)
        len = 4096;
    //Read sound samples from input device to buffer
    unsigned int selectBuffer(myNextBuffer % MaxBufferQUEUE);
    qint64 l = m_input->read(m_buffer[selectBuffer].data(), len);
    if(l > 0)
    {
        //bar.setMaximum(100);
        //bar.setMinimum(0);
        //long avg(0);
        //short *startData((short*)m_buffer[selectBuffer].data());
        //short *endData((short*)m_buffer[selectBuffer].data() + len);
        //while(startData != endData)
        //    avg += *(startData++);
        //avg /= len;
        //qDebug() << avg;
        //bar.setValue((avg * 100 ) / sizeof(short));
        //bar.show();

        //Remove noise using Low Pass filter algortm[Simple algorithm used to remove noise]
        short *buffer = (short*)m_buffer[selectBuffer].data();
        //for(int iIndex(1); iIndex < len; ++iIndex )
        //    resultingData[iIndex] = 0.333f * resultingData[iIndex] + 0.666f * resultingData[iIndex-1];

        long totalSquare(0);
        for (int i(0); i < len; ++i)
        {
            short sample = buffer[i];
            totalSquare += sample * sample;
        }
        long meanSquare = 2 * totalSquare / len;
        long double rms = std::sqrt(meanSquare);
        long double volume = rms / 32768.0;
        //qDebug() << (double)volume * 1000;
        //bar.setValue(volume * 1000);
        //bar.show();
        if(volume * 1000 > MicTheshold || volume != volume)
        {
            QByteArray audioData(m_buffer[selectBuffer].data(), len);
            QByteArray EncodedAudio(AudioEncoderPtr->encode(audioData));
            //m_output->write(m_buffer[selectBuffer], len);
            EncodedAudio.push_front(headerAudio);
            //qDebug() << EncodedAudio.size();
            //m_output->write(m_buffer[selectBuffer], len);
            m_webSocket.sendBinaryMessage(EncodedAudio);
        }
    }
    ++myNextBuffer;
}
void SslClient::imageSend(const QImage* img)
{
    QByteArray imageData((char*)img->bits(), img->height() * img->bytesPerLine());
    unsigned short h(img->height()), w(img->width());
    imageData.push_front(((char*)&h)[0]);
    imageData.push_front(((char*)&h)[1]);
    imageData.push_front(((char*)&w)[0]);
    imageData.push_front(((char*)&w)[1]);
    imageData.push_front(headerImage);
    m_webSocket.sendBinaryMessage(imageData);
}
//! [constructor]

//! [onConnected]
void SslClient::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,this, &SslClient::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived,this, &SslClient::onbinaryMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}

void SslClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
}

//! [onConnected]

//! [onTextMessageReceived]
void SslClient::onTextMessageReceived(QString message)
{
    qDebug() << "Message received:" << message;
    //qApp->quit();
}

void SslClient::onbinaryMessageReceived(QByteArray message)
{
    //qDebug() << (int)message[0];
    if(message[0])
    {
        //qDebug() << "Audio";
        m_output->write(message.data()+1, message.size()-1);
    }
    else
    {
        unsigned short h, w;
        unsigned char *ptr((unsigned char*)&w);
        ptr[0] = message[2];
        ptr[1] = message[1];
        ptr = (unsigned char*)&h;
        ptr[0] = message[4];
        ptr[1] = message[3];
        qDebug() << "Video" << w << h;
        QImage img((unsigned char*)message.data()+5, w, h, w * 3, QImage::Format_RGB888);
        //display.setPixmap(QPixmap::fromImage(img));
        //display.show();
    }
    //qDebug() << "Message received:" << message;
    //qApp->quit();
}

void SslClient::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << errors;
    Q_UNUSED(errors);
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    m_webSocket.ignoreSslErrors();
}
//! [onTextMessageReceived]
