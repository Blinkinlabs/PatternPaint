#ifndef SCENECONFIGURATION_H
#define SCENECONFIGURATION_H

#include <QDialog>

#include "patternwriter.h"

namespace Ui {
class SceneConfiguration;
}

/// Dialog to allow the user to specify a new width/height for the animation
class SceneConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit SceneConfiguration(QWidget *parent = 0);
    ~SceneConfiguration();

    void setOutputSize(QSize size);
    QSize getOutputSize() const;

    void setColorMode(ColorMode mode);
    ColorMode getColorMode() const;
private:
    Ui::SceneConfiguration *ui;
};

#endif // SCENECONFIGURATION_H
