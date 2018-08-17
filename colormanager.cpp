#include "colormanager.h"

#include <QPainter>

QColor ColorManager::getColor(QString element)
{
    int index = elements.indexOf(element);

    if (index>=0) return colors.at(index);

    QColor newColor = getNewColor();
    elements.append(element);
    colors.append(newColor);
    return newColor;
}

QColor ColorManager::getNewColor()
{
    QColor color = defColors.at(elements.count()%defColors.count()).toRgb();

    if (!rgb)
    {
        int r = color.red();
        int b = color.blue();

        color.setRed(b);
        color.setBlue(r);
    }

    return color;
}

bool ColorManager::getRgb() const
{
    return rgb;
}

void ColorManager::setRgb(bool value)
{
    rgb = value;
}

int getColor(QImage mask, QColor color, int x, int y)
{
    return color == Qt::red  ? qRed(mask.pixel(x,y))  :
           color == Qt::blue ? qBlue(mask.pixel(x,y)) :
                               qGreen(mask.pixel(x,y));
}
