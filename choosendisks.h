#ifndef CHOOSENDISKS_H
#define CHOOSENDISKS_H

#include <QDialog>

namespace Ui {
class ChooseNDisks;
}

class ChooseNDisks : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseNDisks(QWidget *parent = nullptr);
    ~ChooseNDisks();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ChooseNDisks *ui;
    int n_;
};

#endif // CHOOSENDISKS_H
