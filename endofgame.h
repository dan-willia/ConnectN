#ifndef ENDOFGAME_H
#define ENDOFGAME_H

#include <QDialog>
#include <QString>

namespace Ui {
class EndOfGame;
}

class EndOfGame : public QDialog
{
    Q_OBJECT

public:
    explicit EndOfGame(const QString &winner, QWidget *parent = nullptr);
    ~EndOfGame();

    void setWinner(const QString &winner);

private slots:
    void on_MainMenuButton_clicked();

private:
    Ui::EndOfGame *ui;
};

#endif // ENDOFGAME_H
