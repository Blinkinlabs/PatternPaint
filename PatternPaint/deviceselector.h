#ifndef DEVICESELECTOR_H
#define DEVICESELECTOR_H

#include <QDialog>

namespace Ui {
class DeviceSelector;
}

class DeviceSelector : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceSelector(QWidget *parent = 0);
    ~DeviceSelector();

private:
    Ui::DeviceSelector *ui;
};

#endif // DEVICESELECTOR_H
