#include "tensorflowthread.h"

void WorkerTF::setTf(Tensorflow *value)
{
    tf = value;
}

void WorkerTF::setImgTF(const QImage &value)
{
    imgTF = value;
}

void WorkerTF::work()
{
    tf->run(imgTF);
    emit results(tf->getKindNetwork(),tf->getResults(),tf->getConfidence(),tf->getBoxes(),tf->getInfTime());
    emit finished();
}

TensorflowThread::TensorflowThread()
{
    threadTF.setObjectName("Tensorflow thread");
    worker.moveToThread(&threadTF);
    QObject::connect(&worker, SIGNAL(results(int,  QStringList, QList<double>, QList<QRectF>, double)), this, SLOT(propagateResults(int, QStringList, QList<double>, QList<QRectF>, double)));
    QObject::connect(&worker, SIGNAL(finished()),  &threadTF, SLOT(quit()));
    QObject::connect(&threadTF, SIGNAL(started()), &worker,   SLOT(work()));
}

void TensorflowThread::stop()
{
    threadTF.exit();
}

void TensorflowThread::setTf(Tensorflow *value)
{
    worker.setTf(value);
}

void TensorflowThread::run(QImage imgTF)
{
    worker.setImgTF(imgTF);
    threadTF.start();
}

void TensorflowThread::propagateResults(int network, QStringList captions, QList<double> confidences, QList<QRectF> boxes, double time)
{
    emit results(network,captions,confidences,boxes,time);
}

