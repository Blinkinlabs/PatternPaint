#ifndef PATTERNUPDATENOTIFIER_H
#define PATTERNUPDATENOTIFIER_H

#include <QObject>

class PatternUpdateNotifier : public QObject
{
    Q_OBJECT
public:
    explicit PatternUpdateNotifier(QObject *parent = 0);
    ~PatternUpdateNotifier();

    void signalDataUpdated() {emit patternDataUpdated();}
    void signalSizeUpdated() {emit patternSizeUpdated();}
    void signalNameUpdated() {emit patternNameUpdated();}

signals:
    void patternDataUpdated();
    void patternSizeUpdated();
    void patternNameUpdated();

public slots:
};

#endif // PATTERNUPDATENOTIFIER_H
