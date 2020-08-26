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
//#include <QApplication>
#include <QtCore/QCoreApplication>
#include "sslclient.h"
#include <iostream>


//#include "cameraframegrabber.h"
//#include <QCamera>
//#include <QCameraViewfinderSettings>

#include "audioencoder.h"

int main(int argc, char *argv[])
{
    //AudioEncoder en(argc, argv);
    AudioEncoder en;
    en.testDecode(argc, argv);
    std::system("pause");
    return 0;
    //en.setAudio(44100, 1, 2);
    //std::system("pause");
    //return 0;
    //QApplication a(argc, argv);
    QCoreApplication a(argc, argv);
    std::string input;
    double soundCap(0.0);
    std::cout << "URL(enter 1 for default): ";
    std::cin >> input;
    if(input == "1")
    {
        input = "wss://192.168.1.77:1234";
        std::cout << input << std::endl;
    }
    std::cout << "Mic: ";
    std::cin >> soundCap;
    SslClient client(QUrl(input.c_str()), soundCap, &en);
    Q_UNUSED(client);


    /*CameraFrameGrabber* viewFinde;
    QCamera* camera;
    QCameraViewfinderSettings* settings;

    QList<QByteArray> cameraDevices = QCamera::availableDevices();
    QByteArray cameraDevice = cameraDevices[0];
    camera = new QCamera(cameraDevice);
    settings = new QCameraViewfinderSettings;
    camera->start();
    QList<QSize> reslutions(camera->supportedViewfinderResolutions());
    QList<QCamera::FrameRateRange> frameRates(camera->supportedViewfinderFrameRateRanges());
    QList<QVideoFrame::PixelFormat> formats(camera->supportedViewfinderPixelFormats());
    QSize largest;
    int largest_area(0);
    int w, h, area;
    int maxminFrameRate(0), maxmaxFrameRate(0);
    for(int i(0); i < reslutions.size(); ++i)
    {
        w = reslutions[i].width();
        h = reslutions[i].height();
        area = w * h;
        if(area > largest_area)
        {
            largest_area = area;
            largest.setWidth(w);
            largest.setHeight(h);
        }
    }
    for(int i(0); i < frameRates.size(); ++i)
    {
        if(frameRates[i].maximumFrameRate > maxmaxFrameRate)
            maxmaxFrameRate = frameRates[i].maximumFrameRate;
        if(frameRates[i].minimumFrameRate > maxminFrameRate)
            maxminFrameRate = frameRates[i].minimumFrameRate;
    }
    settings->setMaximumFrameRate(maxmaxFrameRate);
    settings->setMinimumFrameRate(maxminFrameRate);
    std::cout << largest.width() << 'x' << largest.height() << ' ' << maxmaxFrameRate << 'x' << maxminFrameRate << std::endl;
    settings->setResolution(largest);
    camera->setViewfinderSettings(*settings);
    viewFinde = new CameraFrameGrabber(largest);
    QObject::connect(viewFinde, &CameraFrameGrabber::sendFrame, &client, &SslClient::imageSend);
    camera->setViewfinder(viewFinde);*/
    return a.exec();
}
