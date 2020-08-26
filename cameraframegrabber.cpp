/*#include "cameraframegrabber.h"

CameraFrameGrabber::CameraFrameGrabber(const QSize& frameSize, QObject *parent) :
    QAbstractVideoSurface(parent)
{
    maxFrames = 100;
    frames = 0;
    for(int i(0); i < maxFrames; ++i)
        imgs[i] = QImage(frameSize, QImage::Format_RGB888);
    this->frameSize = frameSize;
    img = &imgs[frames];
    *img = QImage(frameSize, QImage::Format_RGB888);
    h = frameSize.height();
    w = frameSize.width();
    maxSizeWrite = (w * h);
    maxSizeRead = (maxSizeWrite * 4);
}
QList<QVideoFrame::PixelFormat> CameraFrameGrabber::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
        << QVideoFrame::Format_ARGB32
        << QVideoFrame::Format_ARGB32_Premultiplied
        << QVideoFrame::Format_RGB32
        << QVideoFrame::Format_RGB24
        << QVideoFrame::Format_RGB565
        << QVideoFrame::Format_RGB555
        << QVideoFrame::Format_ARGB8565_Premultiplied
        << QVideoFrame::Format_BGRA32
        << QVideoFrame::Format_BGRA32_Premultiplied
        << QVideoFrame::Format_BGR32
        << QVideoFrame::Format_BGR24
        << QVideoFrame::Format_BGR565
        << QVideoFrame::Format_BGR555
        << QVideoFrame::Format_BGRA5658_Premultiplied
        << QVideoFrame::Format_AYUV444
        << QVideoFrame::Format_AYUV444_Premultiplied
        << QVideoFrame::Format_YUV444
        << QVideoFrame::Format_YUV420P
        << QVideoFrame::Format_YV12
        << QVideoFrame::Format_UYVY
        << QVideoFrame::Format_YUYV
        << QVideoFrame::Format_NV12
        << QVideoFrame::Format_NV21
        << QVideoFrame::Format_IMC1
        << QVideoFrame::Format_IMC2
        << QVideoFrame::Format_IMC3
        << QVideoFrame::Format_IMC4
        << QVideoFrame::Format_Y8
        << QVideoFrame::Format_Y16
        << QVideoFrame::Format_Jpeg
        << QVideoFrame::Format_CameraRaw
        << QVideoFrame::Format_AdobeDng;
}
bool CameraFrameGrabber::present(const QVideoFrame &frame)
{
    if(frame.isValid())
    {
        cloneFrame = frame;
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        if(this->surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop)
            flip_image_RGBAToRGB();
        else
            RGBAToRGB();
        emit sendFrame(img);
        cloneFrame.unmap();
        //display.setPixmap(QPixmap::fromImage(*img));
        //display.show();
        img = &imgs[++frames % maxFrames];
        return true;
    }
    return false;
}
void CameraFrameGrabber::flip_image_RGBAToRGB()
{
    unsigned char* out_data(img->bits());
    unsigned char* read_data(cloneFrame.bits()-1);
    unsigned char* end_data(read_data + maxSizeRead);
    while(read_data < end_data)
    {
        //qDebug() << *end_data;
        --end_data;                  //ALPHA
        //qDebug() << *end_data;
        *(out_data++) = *(end_data--); //RED Blue
        //qDebug() << *end_data;
        *(out_data++) = *(end_data--); //GREEN Green
        //qDebug() << *end_data;
        *(out_data++) = *(end_data--);   //BLUE Red
        //qDebug() << *end_data;
        //out_data += 3;
        //std::system("pause");
    }
}
void CameraFrameGrabber::RGBAToRGB()
{
    unsigned char* out_data(img->bits());
    unsigned char* read_data(cloneFrame.bits());
    unsigned char* end_data(read_data + maxSizeRead);
    while(read_data != end_data)
    {
        *(out_data++) = *(read_data++); //Blue
        *(out_data++) = *(read_data++); //Green
        *(out_data++) = *(read_data++); //Red
        ++read_data;
    }
}
*/
