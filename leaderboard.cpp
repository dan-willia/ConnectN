#include "leaderboard.h"
#include "ui_leaderboard.h"
#include "connectnwindow.h"

#include <QDialog>
#include <QFile>
#include <QString>
#include <QVector>

Leaderboard::Leaderboard(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Leaderboard)
{
    ui->setupUi(this);

    QVector<LeaderboardEntry> entries{LoadLeaderboard()};

    ui->Table->setRowCount(entries.size());
    ui->Table->setColumnCount(4);
    ui->Table->setHorizontalHeaderLabels({"Name", "Games Played", "Games Won", "Win %"});

    qDebug() << "got entries:\n";
    for (const auto& entry : entries) {
        qDebug() << entry.name << "," << entry.games_played << "," << entry.games_won << "," << entry.win_perc << "\n";
    }

    for (qsizetype i=0; i<entries.size(); i++) {
        LeaderboardEntry entry{entries[i]};
        ui->Table->setItem(i, 0, new QTableWidgetItem{entry.name});
        ui->Table->setItem(i, 1, new QTableWidgetItem{QString::number(entry.games_played)});
        ui->Table->setItem(i, 2, new QTableWidgetItem{QString::number(entry.games_won)});
        ui->Table->setItem(i, 3, new QTableWidgetItem{QString::number(entry.win_perc * 100) + "%"});
    }
}
Leaderboard::~Leaderboard()
{
    delete ui;
}

QVector<Leaderboard::LeaderboardEntry> Leaderboard::LoadLeaderboard()
{
    QVector<Leaderboard::LeaderboardEntry> entries{};
    QString path{ConnectNWindow::GetLeaderboardPath()};
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open leaderboard";
        return entries;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line{in.readLine()};
        QStringList vals{line.split(',')};
        entries.push_back(LeaderboardEntry{vals[0], QString(vals[1]).toInt(), QString(vals[2]).toInt(), QString(vals[3]).toDouble()});
    }
    file.close();

    // Sort in descending order by win percentage
    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.win_perc > b.win_perc;
    });

    return entries;
}
