#include "endofgame.h"
#include "ui_endofgame.h"
#include <QString>

EndOfGame::EndOfGame(const QString &winner, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EndOfGame)
{
    ui->setupUi(this);
    ui->WinMessage->setText(winner + " wins!");
}

EndOfGame::~EndOfGame()
{
    delete ui;
}

void EndOfGame::setWinner(const QString &winner)
{
    ui->WinMessage->setText(winner + " wins!");
}

void EndOfGame::on_MainMenuButton_clicked()
{
    close();
}

