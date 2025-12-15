#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <QDialog>
#include <QString>
#include <QVector>

namespace Ui {
class Leaderboard;
}

class Leaderboard : public QDialog
{
    Q_OBJECT

public:
    explicit Leaderboard(QWidget *parent = nullptr);

    struct LeaderboardEntry {
        QString name;
        int games_played;
        int games_won;
        double win_perc;
    };

    ~Leaderboard();
    static QVector<LeaderboardEntry> LoadLeaderboard();

private:
    Ui::Leaderboard *ui;
};

#endif // LEADERBOARD_H
