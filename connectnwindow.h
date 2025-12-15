/* connectnwindow.h
 *
 * The main window for the QApplication for ConnectN.
 *
 *
 *
 */
#ifndef CONNECTNWINDOW_H
#define CONNECTNWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <vector>
#include "endofgame.h"
#include "endofround.h"
#include "outoftime.h"
#include "startmenu.h"

class ConnectN;
class Disk;

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectNWindow;
}
QT_END_NAMESPACE

class ConnectNWindow : public QMainWindow
{
    Q_OBJECT

public:
    const QString GAMES_FILE{":/game_data/testgames.csv"};
    const QColor BACKGROUND_COLOR{QColor{"lightblue"}};
    const QColor EMPTY_COLOR{QColor{"white"}};
    const QColor P1_COLOR{QColor{"red"}};
    const QColor P2_COLOR{QColor{"yellow"}};
    const std::vector<QColor> DISK_COLORS { EMPTY_COLOR, P1_COLOR, P2_COLOR };
    const int SCENE_WIDTH = 500;
    const int SCENE_HEIGHT = 540;
    const int FAST_ANIMATION = 100;
    const int NORMAL_ANIMATION = 1000;
    const int ANIMATION_SPEED = NORMAL_ANIMATION;
    const int CONVERT_ANIMATION = 500;
    const int TURN_TIME_LIMIT = 70;
    const int TIMER_UPDATE_INTERVAL = 100;
    const int WIN_GAME_SCORE = 3;

    ConnectNWindow(QWidget *parent = nullptr);
    ~ConnectNWindow();

    void setStartMenu(StartMenu *start) { startmenu = start; }
    void StartNewGame(const std::vector<QString> &params);

    // Load saved game: matches uid from csv file
    void LoadGame(int uid);
    void LoadGame(const QString &filename);

    // Global update of entire Ui to be consistent with underlying game model;
    void UpdateUi();
    void UpdateDisks();
    void UpdateTextUi();
    void UpdateUiAbilities();
    void ResetTimer();

    void AfterDiskAnimation(Disk* to_delete);
    void AfterDiskConversion();
    void AfterClearRow(std::vector<Disk*> to_delete);
    void EndRoundDialog(const QString &winner, int round);
    void EndRoundDialog(int round);
    void EndGameDialog(const QString &winner);

    void HandleConvertDisk(Disk* d);
    void HandleClearRow(Disk* d);
    void HandleClearCol(Disk* d);

    void HandleRoundWin(int winner);
    void HandleDraw();
    void HandleGameWin(const QString &winner, const QString &loser);
    void HandleTimeOut(const QString &next_player);

    int GetGlobalUid();
    void SaveGlobalUid(int global_uid);
    void UpdateLeaderboard(const QString &winner, const QString &loser);

    static QString GetDataDirectory();
    static QString GetGamesDirectory();
    static QString GetLeaderboardPath();

private slots:
    void DiskSelectedSlot(Disk *d);

    void UpdateTurnTimer();

    void on_ClearRowButtonP1_clicked();

    void on_ConvertDiskButtonP1_clicked();

    void on_ClearColumnButtonP1_clicked();

    void on_ClearRowButtonP2_clicked();

    void on_ClearColumnButtonP2_clicked();

    void on_ConvertDiskButtonP2_clicked();

    void on_SaveGameButton_clicked();

    void on_QuitGameButton_clicked();

    void on_EndRoundButton_clicked();

private:
    Ui::ConnectNWindow *ui;
    QGraphicsScene *scene;
    StartMenu *startmenu{nullptr};
    EndOfRound *end_of_round_{nullptr};
    OutOfTime *out_of_time_{nullptr};
    EndOfGame *end_of_game_{nullptr};
    QTimer *turn_timer_;
    int time_remaining_;
    ConnectN *connectn_{nullptr}; // pointer to game instance
    std::vector<std::vector<Disk*>> disks_;
    bool animation_ip_; // boolean indicating whether animation is in progress
    bool clear_row_mode_{false};
    bool clear_col_mode_{false};
    bool convert_disk_mode_{false};
};
#endif // CONNECTNWINDOW_H
