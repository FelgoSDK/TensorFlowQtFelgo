#ifndef TENSORFLOW_H
#define TENSORFLOW_H

#include <QStringList>
#include <QImage>
#include <QRectF>

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/framework/tensor.h"

class Tensorflow
{
public:
    static const int knIMAGE_CLASSIFIER = 1;
    static const int knOBJECT_DETECTION = 2;

    Tensorflow();

    bool init(int imgHeight, int imgWidth);
    void initInput(int imgHeight, int imgWidth);
    double getThreshold() const;
    void setThreshold(double value);
    QStringList getResults();
    QList<double> getConfidence();
    QList<QRectF> getBoxes();
    int getKindNetwork();
    bool run(QImage img);
    QString getModelFilename() const;
    void setModelFilename(const QString &value);
    QString getLabelsFilename() const;
    void setLabelsFilename(const QString &value);
    int getImgHeight() const;
    int getImgWidth() const;
    double getInfTime() const;

private:
    // Fixed image size for image classification
    const int fixed_width  = 224;
    const int fixed_heigth = 224;
    // Output names
    const QString num_detections    = "num_detections";
    const QString detection_classes = "detection_classes";
    const QString detection_scores  = "detection_scores";
    const QString detection_boxes   = "detection_boxes";
    // Output lists
    const std::vector<std::string> listOutputsObjDet = {num_detections.toStdString(),detection_classes.toStdString(),detection_scores.toStdString(),detection_boxes.toStdString()};
    const std::vector<std::string> listOutputsImgCla = {"MobilenetV2/Predictions/Reshape_1"};

    bool initialized;
    double threshold;

    // Results
    QStringList rCaption;
    QList<double> rConfidence;
    QList<QRectF> rBox;
    double infTime;

    int kind_network;
    std::unique_ptr<tensorflow::Session> session;
    std::vector<tensorflow::Tensor> outputs;

    bool inference();
    bool setInputs(QImage image);
    bool getClassfierOutputs(int &index, double &score);
    bool getObjectOutputs(QStringList &captions, QList<double> &confidences, QList<QRectF> &locations);
    bool readLabels();

    QString input_name;
    tensorflow::DataType input_dtype;
    std::unique_ptr<tensorflow::Tensor> input_tensor;
    QString modelFilename;
    QString labelsFilename;
    QStringList labels;
    QString getLabel(int index);
    int img_height, img_width, img_channels;
    const QImage::Format format = QImage::Format_RGB888;
    const int numChannels = 3;
};

#endif // TENSORFLOW_H
