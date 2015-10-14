#ifndef PATTERNUPDATENOTIFIER_H
#define PATTERNUPDATENOTIFIER_H

#include <QObject>

class PatternUpdateNotifier : public QObject
{
    Q_OBJECT
public:
    explicit PatternUpdateNotifier(QObject *parent = 0);
    ~PatternUpdateNotifier();

    void signalModifiedChange() {emit patternModifiedChanged();}
    void signalNameUpdated() {emit patternNameUpdated();}

signals:
    void patternModifiedChanged();
    void patternNameUpdated();

public slots:
};

#endif // PATTERNUPDATENOTIFIER_H
