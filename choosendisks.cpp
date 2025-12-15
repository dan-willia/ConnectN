#include "choosendisks.h"
#include "ui_choosendisks.h"

ChooseNDisks::ChooseNDisks(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseNDisks)
    , n_{}
{
    ui->setupUi(this);
}

ChooseNDisks::~ChooseNDisks()
{
    delete ui;
}

void ChooseNDisks::on_buttonBox_accepted()
{
    n_ = ui->spinBox->value();
    close();
}

