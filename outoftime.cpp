#include "outoftime.h"
#include "ui_outoftime.h"
#include <QString>
#include <QTimer>

OutOfTime::OutOfTime(const QString &next_player, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OutOfTime)
{
    ui->setupUi(this);
    ui->SkippedTurnMessage->setText("Out of time!");
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &OutOfTime::UpdateTimer);
    ResetTimer(next_player);

    qDebug() << "out of time created";
}

OutOfTime::~OutOfTime()
{
    delete ui;
    qDebug() << "out of time destroyed";
}

void OutOfTime::ResetTimer(const QString &next_player)
{
    ui->NextTurnMessage->setText(next_player + "'s turn in ");
    time_remaining_ = TIME_LIMIT;
    ui->Countdown->setText(QString::number(time_remaining_));
    ui->progressBar->setMaximum(TIME_LIMIT);
    ui->progressBar->setValue(TIME_LIMIT);
    timer_->start(TIMER_UPDATE_INTERVAL);
}

void OutOfTime::UpdateTimer()
{
    time_remaining_--;
    ui->progressBar->setValue(time_remaining_);
    ui->Countdown->setText(QString::number(time_remaining_));
    if (time_remaining_ <= 0) {
        timer_->stop();
        close();
    }
}
