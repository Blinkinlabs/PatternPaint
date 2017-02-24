#ifndef TOOLCONFIGURATION_H
#define TOOLCONFIGURATION_H

#include <QObject>
#include <QColor>

class InstrumentConfiguration : public QObject
{
    Q_OBJECT
public:
    QColor getToolColor() const;
    int getPenSize() const;

signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);

private:
    QColor toolColor;      ///< Color of the current drawing tool (TODO: This should be a pointer to a tool)
    int toolSize;          ///< Size of the current drawing tool (TODO: This should be a pointer to a tool)
};

#endif // TOOLCONFIGURATION_H
