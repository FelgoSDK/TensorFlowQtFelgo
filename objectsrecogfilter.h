#ifndef OBJECTSRECOGFILTER_H
#define OBJECTSRECOGFILTER_H

#include <QVideoFilterRunnable>
#include <QMutex>

#include "tensorflow.h"
#include "tensorflowthread.h"

class ObjectsRecogFilterRunable;

class ObjectsRecogFilter : public QAbstractVideoFilter
{
    Q_OBJECT

    Q_PROPERTY(double cameraOrientation READ getCameraOrientation WRITE setCameraOrientation)
    Q_PROPERTY(double videoOrientation READ getVideoOrientation WRITE setVideoOrientation)
    Q_PROPERTY(double minConfidence READ getMinConfidence WRITE setMinConfidence)
    Q_PROPERTY(QSize contentSize READ getContentSize WRITE setContentSize)
    Q_PROPERTY(QString model READ getModel WRITE setModel)
    Q_PROPERTY(bool showTime READ getShowTime WRITE setShowTime)
    Q_PROPERTY(bool ready READ getInitialized() NOTIFY initializedChanged)

private:
    double camOrientation;
    double vidOrientation;
    double minConf;
    bool   running;
    bool   initialized;
    bool   showInfTime;
    QMutex mutex;
    QSize  videoSize;
    QString kindNetwork;
    Tensorflow tf;
    TensorflowThread tft;
    ObjectsRecogFilterRunable *rfr;

signals:
    void runTensorFlow(QImage imgTF);
    void initializedChanged(const bool &value);

public slots:
    void init(int imgHeight, int imgWidth);
    void initInput(int imgHeight, int imgWidth);
    void setRunning(bool value);

private slots:
    void TensorFlowExecution(QImage imgTF);
    void processResults(int network, QStringList res, QList<double> conf, QList<QRectF> boxes, double time);

public:
    ObjectsRecogFilter();
    QVideoFilterRunnable *createFilterRunnable();
    void setCameraOrientation(double o);
    void setVideoOrientation(double o);
    double getCameraOrientation();
    double getVideoOrientation();
    double getMinConfidence() const;
    void setMinConfidence(double value);
    bool getRunning();
    void releaseRunning();
    QSize getContentSize() const;
    void setContentSize(const QSize &value);
    double getImgHeight() const;
    double getImgWidth() const;
    bool getInitialized() const;
    QString getModel() const;
    void setModel(const QString &value);
    bool getShowTime() const;
    void setShowTime(bool value);
};

class ObjectsRecogFilterRunable : public QVideoFilterRunnable
{
    public:
        ObjectsRecogFilterRunable(ObjectsRecogFilter *filter, QStringList res);
        QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);
        void setResults(int net, QStringList res, QList<double> conf, QList<QRectF> box, double time);

    private:
        ObjectsRecogFilter *m_filter;
        int           network;
        QStringList   results;
        QList<double> confidence;
        QList<QRectF> boxes;
        double        infTime;
};

#endif // OBJECTSRECOGFILTER_H
