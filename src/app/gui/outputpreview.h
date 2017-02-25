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

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void setFixture(Fixture* newFixture);
    void setFrameData(int index, const QImage &data);

private:
    QPointer<Fixture> fixture;
    QImage frameData;
};

#endif // OUTPUTPREVIEW_H
