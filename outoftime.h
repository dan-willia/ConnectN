#ifndef OUTOFTIME_H
#define OUTOFTIME_H

#include <QDialog>
#include <QString>
#include <QTimer>

namespace Ui {
class OutOfTime;
}

class OutOfTime : public QDialog
{
    Q_OBJECT

public:
    const int TIME_LIMIT = 3;
    const int TIMER_UPDATE_INTERVAL = 1000;
    explicit OutOfTime(const QString &next_player, QWidget *parent = nullptr);
    ~OutOfTime();

    void ResetTimer(const QString &next_player);

private slots:
    void UpdateTimer();

private:
    Ui::OutOfTime *ui;
    QTimer *timer_{nullptr};
    int time_remaining_;    // seconds
};

#endif // OUTOFTIME_H
