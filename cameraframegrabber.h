/*#ifndef CAMERAFRAMEGRABBER_H
#define CAMERAFRAMEGRABBER_H


// Qt includes
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QLabel>
#include <QDebug>
class CameraFrameGrabber : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit CameraFrameGrabber(const QSize& frameSize, QObject *parent = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);
signals:
    void sendFrame(const QImage* frame);
private:
    void flip_image_RGBAToRGB();
    void RGBAToRGB();
    QSize frameSize;
    QImage imgs[100];
    QImage *img;
    QVideoFrame cloneFrame;
    unsigned int h;
    unsigned int w;
    unsigned int maxSizeWrite;
    unsigned int maxSizeRead;
    QLabel display;
    int frames, maxFrames;
};

#endif // CAMERAFRAMEGRABBER_H
*/
