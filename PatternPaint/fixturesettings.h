#ifndef FIXTURESETTINGS_H
#define FIXTURESETTINGS_H

#include <QDialog>

#include "patternwriter.h"

namespace Ui {
class FixtureSettings;
}

/// Dialog to allow the user to specify a new width/height for the animation
class FixtureSettings : public QDialog
{
    Q_OBJECT

public:
    explicit FixtureSettings(QWidget *parent = 0);
    ~FixtureSettings();

    void setOutputSize(QSize size);
    QSize getOutputSize() const;

    void setColorMode(PatternWriter::ColorMode mode);
    PatternWriter::ColorMode getColorMode() const;
private:
    Ui::FixtureSettings *ui;
};

#endif // FIXTURESETTINGS_H
