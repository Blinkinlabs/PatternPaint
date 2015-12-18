#ifndef OUTPUTPREVIEW_H
#define OUTPUTPREVIEW_H

#include <fixture.h>

#include <QObject>
#include <QPointer>
#include <QWidget>

class OutputPreview : public QWidget
{
    Q_OBJECT
public:
    explicit OutputPreview(QWidget *parent = 0);
    ~OutputPreview();

    void setFixture(Fixture* newFixture);
    void setFrameData(int index, const QImage data);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QPointer<Fixture> fixture;

    QList<QPoint> outputLocations;
    QList<QColor> colorStream;
};

#endif // OUTPUTPREVIEW_H
