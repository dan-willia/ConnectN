#ifndef STARTMENU_H
#define STARTMENU_H

#include <QDialog>
#include <QString>
#include <vector>
#include "leaderboard.h"

namespace Ui {
class StartMenu;
}

class StartMenu : public QDialog
{
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();
    enum class Choice {NewGame, LoadGame, ViewLeaderboard};
    Choice get_choice() const { return choice_; }
    QString get_filename() const { return filename_; }
    std::vector<QString> getNewGameParameters() const;
private slots:
    void on_NewGameButton_clicked();

    void on_LoadGameButton_clicked();

    void on_ViewLeaderboardButton_clicked();

private:
    Ui::StartMenu *ui;
    int n_{};
    QString p1_name_;
    QString p2_name_;
    QString filename_;
    Choice choice_;
    Leaderboard* leaderboard_;
};

#endif // STARTMENU_H
