#include "instrumentconfiguration.h"

void InstrumentConfiguration::setToolColor(QColor color)
{
    toolColor = color;
}

void InstrumentConfiguration::setToolSize(int size)
{
    toolSize = size;
}

QColor InstrumentConfiguration::getToolColor() const
{
    return toolColor;
}

int InstrumentConfiguration::getPenSize() const
{
    return toolSize;
}
