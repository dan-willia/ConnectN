#include "endofround.h"
#include "ui_endofround.h"

EndOfRound::EndOfRound(const QString &winner, int round, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EndOfRound)
{
    ui->setupUi(this);
    ui->WinMessage->setText(winner + " wins round " + QString::number(round) + "!");
}

EndOfRound::EndOfRound(int round, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EndOfRound)
{
    ui->setupUi(this);
    ui->WinMessage->setText("Round " + QString::number(round) + " is a draw!");
}

EndOfRound::~EndOfRound()
{
    delete ui;
}

void EndOfRound::setWinner(const QString &winner, int round)
{
    ui->WinMessage->setText(winner + " wins round " + QString::number(round) + "!");
}

void EndOfRound::setDraw(int round)
{
    ui->WinMessage->setText("Round " + QString::number(round) + " is a draw!");
}

void EndOfRound::on_NextRoundButton_clicked()
{
    close();
}

