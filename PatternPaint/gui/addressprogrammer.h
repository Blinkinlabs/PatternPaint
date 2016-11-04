#ifndef ADDRESSPROGRAMMER_H
#define ADDRESSPROGRAMMER_H

#include <QDialog>

namespace Ui {
class AddressProgrammer;
}

/// Dialog to enable the address programmer feature for LightBuddy controllers (program WS2821/WS2822s LEDs)
class AddressProgrammer : public QDialog
{
    Q_OBJECT

public:
    explicit AddressProgrammer(QWidget *parent = 0);
    ~AddressProgrammer();

private:
    Ui::AddressProgrammer *ui;
};

#endif // ADDRESSPROGRAMMER_H
