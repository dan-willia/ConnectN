#ifndef ENDOFROUND_H
#define ENDOFROUND_H

#include <QDialog>
#include <QString>

namespace Ui {
class EndOfRound;
}

class EndOfRound : public QDialog
{
    Q_OBJECT

public:
    explicit EndOfRound(const QString &winner, int round, QWidget *parent = nullptr);
    explicit EndOfRound(int round, QWidget *parent = nullptr);
    ~EndOfRound();

    void setWinner(const QString &winner, int round);
    void setDraw(int round);

private slots:
    void on_NextRoundButton_clicked();

private:
    Ui::EndOfRound *ui;
};

#endif // ENDOFROUND_H
