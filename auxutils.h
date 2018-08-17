#ifndef AUXUTILS_H
#define AUXUTILS_H

#include <QVideoFrame>
#include <QByteArray>
#include <QFont>

class AuxUtils
{
public:
    static QString resolveModelFilePath(QString modelName);
    static QString getAssetsPath();
    static bool fileExist(QString filename);
    static QString copyIfNotExistOrUpdate(QString file, QString defFile);
    static bool copyFile(QString origin, QString destination);
    static void deleteFile(QString filename);
    static QByteArray fileMD5(QString filename);
    static QString urlToFile(QString path);
    static bool isBGRvideoFrame(QVideoFrame f);
    static QImage rotateImage(QImage img, double rotation);
    static QImage drawText(QImage image, QRectF rect, QString text, Qt::AlignmentFlag pos = Qt::AlignBottom,
                           Qt::GlobalColor borderColor = Qt::black,
                           double borderSize = 0.5,
                           Qt::GlobalColor fontColor = Qt::white,
                           QFont font = QFont("Times", 16, QFont::Bold));
    static QRectF frameMatchImg(QImage img, QSize rectSize);
    static QImage drawBoxes(QImage image, QRect rect, QStringList captions, QList<double> confidences, QList<QRectF> boxes, double minConfidence, bool rgb);
    static double dpi();
    static double sp(double pixel);

private:
    // Constant values
    static constexpr double  FONT_PIXEL_SIZE_TEXT = 44;
    static constexpr double  FONT_PIXEL_SIZE_BOX  = 28;
    static constexpr double  LINE_WIDTH           = 2;
    static constexpr int     FONT_HEIGHT_MARGIN   = 3;
    static constexpr int     FONT_WIDTH_MARGIN    = 6;
};

#endif // AUXUTILS_H
