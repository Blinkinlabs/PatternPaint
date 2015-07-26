#ifndef PATTERNUPDATENOTIFIER_H
#define PATTERNUPDATENOTIFIER_H

#include <QObject>

class PatternUpdateNotifier : public QObject
{
    Q_OBJECT
public:
    explicit PatternUpdateNotifier(QObject *parent = 0);
    ~PatternUpdateNotifier();

    void signalUpdated() {emit updated();}

signals:
    void updated();

public slots:
};

#endif // PATTERNUPDATENOTIFIER_H
