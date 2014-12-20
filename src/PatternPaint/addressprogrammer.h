#ifndef ADDRESSPROGRAMMER_H
#define ADDRESSPROGRAMMER_H

#include <QDialog>

namespace Ui {
class AddressProgrammer;
}

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
