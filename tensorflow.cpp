#include "tensorflow.h"

#include <QFile>
#include <QElapsedTimer>
#include <QDebug>

#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/cc/ops/const_op.h"
using namespace tensorflow;

#include "get_top_n.h"

Tensorflow::Tensorflow()
{
    initialized = false;
}

double Tensorflow::getThreshold() const
{
    return threshold;
}

void Tensorflow::setThreshold(double value)
{
    threshold = value;
}

QStringList Tensorflow::getResults()
{
    return rCaption;
}

QList<double> Tensorflow::getConfidence()
{
    return rConfidence;
}

QList<QRectF> Tensorflow::getBoxes()
{
    return rBox;
}

int Tensorflow::getKindNetwork()
{
    return kind_network;
}

QString Tensorflow::getModelFilename() const
{
    return modelFilename;
}

void Tensorflow::setModelFilename(const QString &value)
{
    modelFilename = value;
}

QString Tensorflow::getLabelsFilename() const
{
    return labelsFilename;
}

void Tensorflow::setLabelsFilename(const QString &value)
{
    labelsFilename = value;
}

int Tensorflow::getImgHeight() const
{
    return img_height;
}

int Tensorflow::getImgWidth() const
{
    return img_width;
}

double Tensorflow::getInfTime() const
{
    return infTime;
}

bool Tensorflow::init(int imgHeight, int imgWidth)
{
    if (getModelFilename().trimmed().isEmpty() || getLabelsFilename().trimmed().isEmpty()) return false;

    GraphDef graph_def;

    // No inference time yet
    infTime = -1;

    // Initial kind of network
    kind_network = knIMAGE_CLASSIFIER;

    // Load labels
    readLabels();

    // Load graph
    Status load_graph_status = ReadBinaryProto(Env::Default(), getModelFilename().toStdString().c_str(), &graph_def);
    if (!load_graph_status.ok()) load_graph_status = ReadTextProto(Env::Default(), getModelFilename().toStdString().c_str(), &graph_def);
    if (!load_graph_status.ok())
    {
        qDebug() << QString::fromStdString(load_graph_status.error_message());
        return false;
    }

    // Create session
    (&session)->reset(NewSession(SessionOptions()));
    Status session_create_status = session->Create(graph_def);
    if (!session_create_status.ok())
    {
        qDebug() << QString::fromStdString(session_create_status.error_message());
        return false;
    }

    // Find input nodes
    std::vector<const tensorflow::NodeDef*> placeholders;

    for (const auto& node : graph_def.node())
    {
        if (node.op() == "Placeholder")
            placeholders.push_back(&node);
        else if (node.name() == detection_boxes.toStdString().c_str())
            kind_network = knOBJECT_DETECTION;
    }

    // Check there are inputs
    if (placeholders.empty()) return false;

    // Get first input
    const tensorflow::NodeDef* input = placeholders.at(0);

    // Get input name & type
    input_name  = QString::fromUtf8(input->name().c_str());
    input_dtype = input->attr().at("dtype").type();

    // Initialize input tensor
    initInput(imgHeight,imgWidth);

    qDebug() << "Neural network loaded";

    initialized = true;

    return initialized;
}

void Tensorflow::initInput(int imgHeight, int imgWidth)
{   
    // Create input tensor
    tensorflow::TensorShape input_shape;
    input_shape.AddDim(1); // Batch size
    input_shape.AddDim((kind_network == knIMAGE_CLASSIFIER ? fixed_heigth : imgHeight)); // Img height
    input_shape.AddDim((kind_network == knIMAGE_CLASSIFIER ? fixed_width  : imgWidth));  // Img width
    input_shape.AddDim(numChannels); // Img channels
    input_tensor.reset(new tensorflow::Tensor(input_dtype, input_shape));
}

bool Tensorflow::readLabels()
{
    if (!labelsFilename.trimmed().isEmpty())
    {
        QFile textFile(labelsFilename);

        if (textFile.exists())
        {
            QByteArray line;

            labels.clear();
            textFile.open(QIODevice::ReadOnly);

            line = textFile.readLine().trimmed();
            while(!line.isEmpty()) // !textFile.atEnd() &&
            {
                labels.append(line);
                line = textFile.readLine().trimmed();
            }

            textFile.close();
        }
        return true;
    }
    return false;
}

bool Tensorflow::run(QImage img)
{
    if (initialized)
    {
        // Transform image format & copy data
        QImage image = img.format() == format ? img : img.convertToFormat(format);

        // Store original image properties
        img_width    = kind_network == knIMAGE_CLASSIFIER ? fixed_width  : image.width();
        img_height   = kind_network == knIMAGE_CLASSIFIER ? fixed_heigth : image.height();
        img_channels = numChannels;

        // Set inputs
        if (!setInputs(image)) return false;

        // Perform inference
        if (!inference()) return false;

        // Clear previous outputs
        rCaption.clear();
        rConfidence.clear();
        rBox.clear();

        // Image classifier
        if (kind_network == knIMAGE_CLASSIFIER)
        {
            int index;
            double score;

            if (!getClassfierOutputs(index,score)) return false;

            rConfidence.append(score);
            rCaption.append(getLabel(index));
        }
        // Object detection
        else if (kind_network == knOBJECT_DETECTION)
            if (!getObjectOutputs(rCaption,rConfidence,rBox)) return false;
        return true;
    }
    return false;
}

QString Tensorflow::getLabel(int index)
{
    if(index>=0 && index<labels.count())
    {
        QString label = labels[index];

        // Capitalize label
        return  label.left(1).toUpper()+label.mid(1);
    }
    return "";
}
bool Tensorflow::inference()
{
    QElapsedTimer timer;
    Status run_status;
    std::vector<std::string>listOutputs = kind_network == knIMAGE_CLASSIFIER ? listOutputsImgCla : listOutputsObjDet;

    timer.start();
    run_status = session->Run({},{{input_name.toStdString(), *input_tensor}},listOutputs,{},&outputs,{});
    infTime = timer.elapsed();
    if (!run_status.ok()) qDebug() << QString::fromStdString(run_status.error_message());
    return run_status.ok();
}

template<class T>
bool formatImageTF(T* out, QImage image, int image_channels, bool input_floating)
{
    const float input_mean = 127.5f;
    const float input_std  = 127.5f;

    // Number of pixels
    const int numberPixels = image.height()*image.width()*image_channels;

    // Pointer to image data
    const uint8_t *output = image.bits();

    // Boolean to [0,1]
    const int inputFloat = input_floating ? 1 : 0;
    const int inputInt   = input_floating ? 0 : 1;

    // Transform to [-128,128]
    for (int i = 0; i < numberPixels; i++)
    {
      out[i] = inputFloat*((output[i] - input_mean) / input_std) +
               inputInt*(uint8_t)output[i];
    }

    return true;
}

bool Tensorflow::setInputs(QImage image)
{
    // For image classification resize
    if (kind_network == knIMAGE_CLASSIFIER)
        image = image.scaled(img_width,img_height);


    // QImage to data
    switch (input_dtype)
    {
        case tensorflow::DT_FLOAT:
        {
            float* data = input_tensor->flat<float>().data();
            formatImageTF<float>(data,image,img_channels,true);
            break;
        }

        case tensorflow::DT_UINT8:
        {
            uint8_t* data = input_tensor->flat<uint8_t>().data();
            formatImageTF<uint8_t>(data,image,img_channels,false);
            break;
        }
        break;
        default: return false;
    }
    return true;
}

template<typename T>
const T* TensorData(const tensorflow::Tensor& tensor, int batch_index);

template<>
const float* TensorData(const tensorflow::Tensor& tensor, int batch_index)
{
    int nelems = tensor.dim_size(1) * tensor.dim_size(2) * tensor.dim_size(3);
    switch (tensor.dtype())
    {
        case tensorflow::DT_FLOAT:
            return tensor.flat<float>().data() + nelems * batch_index;
        default:
            LOG(FATAL) << "Should not reach here!";
    }
    return nullptr;
}

template<>
const uint8_t* TensorData(const tensorflow::Tensor& tensor, int batch_index)
{
    int nelems = tensor.dim_size(1) * tensor.dim_size(2) * tensor.dim_size(3);
    switch (tensor.dtype())
    {
        case tensorflow::DT_UINT8:
            return tensor.flat<uint8_t>().data() + nelems * batch_index;
        default:
            LOG(FATAL) << "Should not reach here!";
    }
    return nullptr;
}

// TODO: naive algorithm
bool Tensorflow::getClassfierOutputs(int &index, double &score)
{
    const int imagePos = 0;

    index = -1;
    score = 0;

    if (outputs.size()>0)
    {
        for (auto &t : outputs)
        {
            tensorflow::TTypes<float, 2>::Tensor scores = t.flat_inner_dims<float>();
            auto dims = scores.dimensions();
            int classesCount = dims[1];
            for(int i = 1; i<classesCount; i++)
            {
                float val = scores(imagePos,i);
                if(val > score)
                {
                    score = val;
                    index = i;
                }
            }
        }
        return true;
    }
    return false;
}

bool Tensorflow::getObjectOutputs(QStringList &captions, QList<double> &confidences, QList<QRectF> &locations)
{
    if (outputs.size() >= 4)
    {
        const int    num_detections    = *TensorData<float>(outputs[0], 0);
        const float* detection_classes =  TensorData<float>(outputs[1], 0);
        const float* detection_scores  =  TensorData<float>(outputs[2], 0);
        const float* detection_boxes   =  TensorData<float>(outputs[3], 0);

        for (int i=0; i<num_detections; i++)
        {
            // Get class
            const int cls = detection_classes[i];

            // Ignore first one
            if (cls == 0) continue;

            // Get score
            float score = detection_scores[i];

            // Check minimum score
            if (score < getThreshold()) break;

            // Get class label
            const QString label = getLabel(cls);

            // Get coordinates
            const float top    = detection_boxes[4 * i]     * img_height;
            const float left   = detection_boxes[4 * i + 1] * img_width;
            const float bottom = detection_boxes[4 * i + 2] * img_height;
            const float right  = detection_boxes[4 * i + 3] * img_width;

            // Save coordinates
            QRectF box(left,top,right-left,bottom-top);

            // Save remaining data
            captions.append(label);
            confidences.append(score);
            locations.append(box);
        }
        return true;
    }
    return false;
}
