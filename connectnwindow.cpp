#include "connectnwindow.h"
#include "ui_connectnwindow.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QInputDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QVector>
#include <string>
#include <vector>
#include <iostream>

#include "connectn.h"
#include "connectnfactory.h"
#include "disk.h"
#include "diskfactory.h"
#include "endofround.h"
#include "endofgame.h"
#include "leaderboard.h"
#include "outoftime.h"
#include "player.h"

ConnectNWindow::ConnectNWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConnectNWindow)
    , animation_ip_{false}
{
    ui->setupUi(this);
    QGraphicsView *view = ui->BoardView;
    view->setBackgroundBrush(QBrush(BACKGROUND_COLOR));
    scene = new QGraphicsScene(this);
    view->setScene(scene);
    view->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    turn_timer_ = new QTimer(this);
    connect(turn_timer_, &QTimer::timeout, this, &ConnectNWindow::UpdateTurnTimer);
}

ConnectNWindow::~ConnectNWindow()
{
    if (connectn_ != nullptr)
        delete connectn_;
    delete ui;
}

void ConnectNWindow::StartNewGame(const std::vector<QString> &params)
{
    // Create game instance
    if (connectn_ != nullptr) { // delete previous game
        delete connectn_;
        qDebug() << "deleted previous game";

        for (auto &row : disks_) {
            for (Disk* d : row) {
                disconnect(d, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
                scene->removeItem(d);
            }
        }
        qDebug() << "disconnected and removed previous disks from scene";
    }

    int global_uid = GetGlobalUid();
    ConnectN::set_global_uid(global_uid);
    SaveGlobalUid(++global_uid);

    connectn_ = new ConnectN{params[0].toInt(), params[1].toStdString(), params[2].toStdString()};

    // Create disks
    disks_ = DiskFactory::CreateDisks(*connectn_->get_board(), connectn_->get_n(), SCENE_HEIGHT, DISK_COLORS, this);

    // Connect disks to ConnectNWindow; add them to scene
    for (auto &row : disks_) {
        for (Disk* d : row) {
            connect(d, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
            scene->addItem(d);
        }
    }

    UpdateUi();
}

void ConnectNWindow::LoadGame(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "File failed to open:" << filename;
    }
    QTextStream in(&file);
    QString line{in.readLine()};
    QStringList q_args = line.split(',');
    std::vector<std::string> std_std_args{};
    for (qsizetype i=0; i<q_args.length(); i++) {
        std_std_args.push_back(q_args[i].toStdString());
    }

    // Create game instance
    if (connectn_ != nullptr) { // delete previous game
        delete connectn_;
        qDebug() << "deleted previous game";

        for (auto &row : disks_) {
            for (Disk* d : row) {
                disconnect(d, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
                scene->removeItem(d);
            }
        }
        qDebug() << "disconnected and removed previous disks from scene";
    }
    connectn_ = ConnectNFactory::CreateGame(std_std_args);

    // Create disks
    disks_ = DiskFactory::CreateDisks(*connectn_->get_board(), connectn_->get_n(), SCENE_HEIGHT, DISK_COLORS, this);

    // Connect disks to ConnectNWindow; add them to scene
    for (auto &row : disks_) {
        for (Disk* d : row) {
            connect(d, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
            // connect(d, &Disk::ConvertDisk, this, &ConnectNWindow::ConvertDiskSlot);
            scene->addItem(d);
        }
    }
    UpdateUi();
}

void ConnectNWindow::LoadGame(int uid)
{
    QFile file{GAMES_FILE};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File failed to open.";
        return;
    }
    QTextStream in(&file);

    // Skip header
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList q_args = line.split(',');

        // Find game with matching uid
        if (q_args[0].toInt() == uid) {
            // copy to vector of std::string
            std::vector<std::string> std_string_args{};
            for (qsizetype i=0; i<q_args.length(); i++) {
            // for (const QString &qs : q_args) {
                std_string_args.push_back(q_args[i].toStdString());
            }

            int global_uid = GetGlobalUid();
            ConnectN::set_global_uid(global_uid);
            SaveGlobalUid(++global_uid);

            // Create game instance
            connectn_ = ConnectNFactory::CreateGame(std_string_args);

            // Create disks
            disks_ = DiskFactory::CreateDisks(*connectn_->get_board(), connectn_->get_n(), SCENE_HEIGHT, DISK_COLORS, this);

            // Connect disks to ConnectNWindow; add them to scene
            for (auto &row : disks_) {
                for (Disk* d : row) {
                    connect(d, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
                    // connect(d, &Disk::ConvertDisk, this, &ConnectNWindow::ConvertDiskSlot);
                    scene->addItem(d);
                }
            }

            return;
        }
    }
    qDebug() << "Game " << uid << " not found";
    return;
}

/* Update and redraw everything in window that depends on the underlying ConnectN model so that it is consistent
 * with that model.
 * Also reset the timer.
 */
void ConnectNWindow::UpdateUi()
{
    UpdateTextUi();
    UpdateDisks();
    ResetTimer();
}

/* Update and redraw disks_ so that it is consistent with model.
 */
void ConnectNWindow::UpdateDisks()
{
    qDebug() << "Entering update disks";
    Board* board = connectn_->get_board();
    for (size_t row = 0; row < board->get_rows(); row++) {
        for (size_t col = 0; col < board->get_cols(); col++) {
            // qDebug() << "Accessing disks_[" << row << "][" << col << "]";
            if (disks_[row][col] != nullptr && board->get_board()[row][col] != disks_[row][col]->get_player()) {
                // qDebug() << "I'm updating the disk";
                disks_[row][col]->set_player(board->get_board()[row][col]);
                disks_[row][col]->set_color(DISK_COLORS[board->get_board()[row][col]]);
            }
        }
    }
    scene->update();
    qDebug() << "Leaving update disks";
}

/* Update all Ui text-based Ui widgets, including turn, score, round and player abilities.
 */
void ConnectNWindow::UpdateTextUi()
{
    Player* current{connectn_->get_whose_turn()};

    // Update turn label
    ui->PlayerTurnLabel->setText(QString::fromStdString(current->get_name()) + "'s turn!");

    // Update scores labels
    std::vector<int> score = connectn_->get_score();
    QString draw_score = QString::number(score[0]);
    QString p1_score = QString::number(score[1]);
    QString p2_score = QString::number(score[2]);
    ui->DrawsLabel->setText("Draws: " + draw_score);
    ui->Player1ScoreLabel->setText(QString::fromStdString(connectn_->get_p1()->get_name()) + ": " + p1_score);
    ui->Player2ScoreLabel->setText(QString::fromStdString(connectn_->get_p2()->get_name()) + ": " + p2_score);

    // Update round label
    ui->CurrentRoundLabel->setText("Round " + QString::number(connectn_->get_round()));

    // Update abilities
    UpdateUiAbilities();
}

/* Player abilities show up in green if they are available and red if they are not.
 */
void ConnectNWindow::UpdateUiAbilities() {
    Player* p1{connectn_->get_p1()};
    Player* p2{connectn_->get_p2()};

    // Player 1
    if (p1->can_clear_row()) {
        ui->ClearRowButtonP1->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ClearRowButtonP1->setStyleSheet("QPushButton { background-color: red; }");
    }

    if (p1->can_clear_col()) {
        ui->ClearColumnButtonP1->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ClearColumnButtonP1->setStyleSheet("QPushButton { background-color: red; }");
    }

    if (p1->can_convert()) {
        ui->ConvertDiskButtonP1->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ConvertDiskButtonP1->setStyleSheet("QPushButton { background-color: red; }");
    }

    // Player 2
    if (p2->can_clear_row()) {
        ui->ClearRowButtonP2->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ClearRowButtonP2->setStyleSheet("QPushButton { background-color: red; }");
    }

    if (p2->can_clear_col()) {
        ui->ClearColumnButtonP2->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ClearColumnButtonP2->setStyleSheet("QPushButton { background-color: red; }");
    }

    if (p2->can_convert()) {
        ui->ConvertDiskButtonP2->setStyleSheet("QPushButton { background-color: green; }");
    } else {
        ui->ConvertDiskButtonP2->setStyleSheet("QPushButton { background-color: red; }");
    }
}

/* Reset and restart the turn timer.
 */
void ConnectNWindow::ResetTimer()
{
    time_remaining_ = TURN_TIME_LIMIT;
    ui->progressBar->setMaximum(TURN_TIME_LIMIT);
    ui->progressBar->setValue(TURN_TIME_LIMIT);
    turn_timer_->start(TIMER_UPDATE_INTERVAL);
}

/* Entry point for handling different scenarios when player clicks on a disk.
 * If no special modes are activated, then a new disk is created which falls into
 * the proper place depending on which column the player clicked.
 *
 * If a disk is clicked on before the previous animation has completed, nothing happens.
 *
 * If one of the special ability modes are activte, then the appropriate handler is called.
 *
 * @param d Pointer to disk that was clicked on.
 */
void ConnectNWindow::DiskSelectedSlot(Disk *d)
{
    if (animation_ip_) { return; }
    if (convert_disk_mode_) { HandleConvertDisk(d); return; }
    if (clear_row_mode_) { HandleClearRow(d); return; }
    if (clear_col_mode_) { HandleClearCol(d); return; }

    animation_ip_ = true;

    // Find the disk that will be updated/deleted
    int col = d->get_col();
    int highest_empty_row = connectn_->get_board()->GetHighestEmptyRow(col);
    if (highest_empty_row == -1) { // column is full
        animation_ip_ = false;
        return;
    }
    Disk* target = disks_[highest_empty_row][col];

    // Make a new disk which will be animated falling into place
    // clone is same as target except with player number and color
    QColor color{};
    int player{};
    switch (connectn_->whose_turn()) {
        case 1: color = P1_COLOR; player = 1; break;
        case 2: color = P2_COLOR; player = 2; break;
    }
    std::cout << "before update:\n " << *(connectn_->get_board()) << std::endl;
    Disk *clone = new Disk{target->x(), target->y(),
                           target->get_row(), target->get_col(),
                           target->get_width(), color, player,
                           this};

    //  Replace target with clone
    disks_[highest_empty_row][col] = clone;

    // Add new disk to scene and connect it to the window
    scene->addItem(clone);
    connect(clone, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);

    // Update model
    connectn_->get_board()->set_pos({highest_empty_row, col}, player);

    std::cout << "board updated:\n " << *(connectn_->get_board()) << std::endl;

    // Set up animation of clone falling into place
    QPropertyAnimation *animation = new QPropertyAnimation(clone, "y");
    animation->setDuration(ANIMATION_SPEED);
    animation->setStartValue(0);
    animation->setEndValue(target->y());
    animation->setEasingCurve(QEasingCurve::InQuad);

    // When animation finishes, target is removed from the scene
    // Use a lambda function to capture the parameters, since finished signal has no parameters
    connect(animation, &QPropertyAnimation::finished, this, [this, target]() {
        AfterDiskAnimation(target);});

    // Also clean up the animation itself
    connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);

    // Run the animation
    animation->start();
}

void ConnectNWindow::HandleConvertDisk(Disk *d)
{
    int whose_turn = connectn_->whose_turn();

    // If disk already belongs to whose turn it is, do nothing
    if (d->get_player() == whose_turn) { return; }

    animation_ip_ = true;

    // Update model
    connectn_->get_whose_turn()->ConvertDisk(
        connectn_->get_board(), {d->get_row(),d->get_col()});

    // Change ownership and color of disk
    QColor old_color{d->get_color()};
    QColor new_color{DISK_COLORS[whose_turn]};
    d->set_player(whose_turn);
    d->set_color(new_color);

    // Create color change animation
    QPropertyAnimation *animation = new QPropertyAnimation(d, "color_", this);
    animation->setDuration(CONVERT_ANIMATION); // 1 second
    animation->setStartValue(old_color);
    animation->setEndValue(new_color);

    // Update global animation status and check win conditions after animation
    connect(animation, &QPropertyAnimation::finished, this, &ConnectNWindow::AfterDiskConversion);

    // Clean up animation
    connect(animation, &QPropertyAnimation::finished,
            animation, &QPropertyAnimation::deleteLater);

    // Run animation
    animation->start();
}

void ConnectNWindow::HandleClearRow(Disk *d)
{
    animation_ip_ = true;

    // Update model
    int row_to_clear = d->get_row();
    Board* board{connectn_->get_board()};
    connectn_->get_whose_turn()->ClearRow(board, row_to_clear);

    size_t board_cols = board->get_cols();

    // First clear the row and immediately update scene
    for (size_t col = 0; col < board_cols; col++) {
        disks_[row_to_clear][col]->set_player(0);
        disks_[row_to_clear][col]->set_color(EMPTY_COLOR);
    }
    scene->update();

    // Set up animation group
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    std::vector<Disk*> to_delete{};

    // Then animate
    // every non-empty disk in the row_to_clear and below need to be animated, except row 0 (these will be empty)
    for (size_t row = row_to_clear; row > 0; row--) {
        for (size_t col = 0; col < board_cols; col++) {
            Disk* current = disks_[row][col];
            Disk* above = disks_[row-1][col];
            if (above->get_player() != 0) {
                // Animate
                qDebug() << "got disk at " << row << ", " << col << " to be animated";

                // Create a clone of the current disk with the color/player of the one
                // in the row immediately below
                QColor color{}; int player{};
                switch (above->get_player()) {
                    case 1: color = P1_COLOR; player = 1; break;
                    case 2: color = P2_COLOR; player = 2; break;
                }
                Disk* clone{current->Clone(player, color)};
                // Replace current disk with clone
                disks_[row][col] = clone;

                // Add it to the scene and connect to window
                scene->addItem(clone);
                connect(clone, &Disk::DiskSelected, this, &ConnectNWindow::DiskSelectedSlot);
                to_delete.push_back(current);

                // Set up animation
                QPropertyAnimation *animation = new QPropertyAnimation(clone, "y", this);
                animation->setDuration(ANIMATION_SPEED);
                animation->setStartValue(above->y());
                animation->setEndValue(current->y());
                animation->setEasingCurve(QEasingCurve::InQuad);

                // Clean up the animation when finished
                connect(animation, &QPropertyAnimation::finished,
                        animation, &QPropertyAnimation::deleteLater);

                // Add animation to animation group
                group->addAnimation(animation);
            }
        }
    }

    // Clean up old disks after animation and check for win conditions
    connect(group, &QParallelAnimationGroup::finished, this, [this, to_delete]() {
        AfterClearRow(to_delete);
    });

    group->start();
    qDebug() << "exiting handle clear row";
}

void ConnectNWindow::HandleClearCol(Disk *d)
{
    animation_ip_ = true; // Pause timer

    // Update model
    int col_to_clear = d->get_col();
    Board* board{connectn_->get_board()};
    connectn_->get_whose_turn()->ClearCol(board, col_to_clear);

    // Clearing a column can't result in a win, so no need to check win conditions
    connectn_->NextTurn();

    // Update Ui
    UpdateUi();

    // Reset mode and cursor
    clear_col_mode_ = false;
    ui->centralwidget->setCursor(Qt::ArrowCursor);

    animation_ip_ = false;
}

void ConnectNWindow::AfterClearRow(std::vector<Disk*> to_delete)
{
    for (Disk* d : to_delete) {
        scene->removeItem(d);
        delete d;
        qDebug() << "old disk deleted";
    }

    // Check win condition
    qDebug() << "Checking win condition";
    Board* board{connectn_->get_board()};
    int winner{board->CheckWin()};
    if (winner) {
        HandleRoundWin(winner);
    } else if (board->CheckDraw()) {
        // Check Draw
        qDebug() << "Checking draw";
        HandleDraw();
    } else {
        // Neither win nor draw: next turn
        qDebug() << "Advancing to next turn";
        connectn_->NextTurn();
        UpdateUi();
    }
    // Reset mode and cursor
    clear_row_mode_ = false;
    ui->centralwidget->setCursor(Qt::ArrowCursor);
    animation_ip_ = false;
    qDebug() << "Leaving after animation...";
}

/* The old disk is deleted after the animation.
 * Check the win conditions after the animation is complete, or there will be strange
 * behavior in the next round. Namely, the first disk will always be placed in the last
 * column played of the previous round, regardless of where the user clicks in the new round.
 * I suspect this is because the round win modal dialog blocks, and so the previous disk-selected
 * signal doesn't get cleaned up properly.
 */
void ConnectNWindow::AfterDiskAnimation(Disk *to_delete)
{
    // Remove old disk
    scene->removeItem(to_delete);
    delete to_delete;
    qDebug() << "old disk deleted";

    // Check win condition
    qDebug() << "Checking win condition";
    Board* board{connectn_->get_board()};
    int winner{board->CheckWin()};
    if (winner) {
        HandleRoundWin(winner);
    } else if (board->CheckDraw()) {
        // Check Draw
        qDebug() << "Checking draw";
        HandleDraw();
    } else {
        // Neither win nor draw: next turn
        qDebug() << "Advancing to next turn";
        connectn_->NextTurn();
        UpdateUi();
    }
    animation_ip_ = false;
    qDebug() << "Leaving after animation...";
}

// Check for win/draws; otherwise advance to next turn
void ConnectNWindow::AfterDiskConversion()
{
    std::cout << "printing board after disk conversion " << std::endl;
    std::cout << *(connectn_->get_board()) << std::endl;
    Board* board{connectn_->get_board()};
    int winner{board->CheckWin()};
    if (winner) {
        HandleRoundWin(winner);
    } else if (board->CheckDraw()) {
        HandleDraw();
    } else {
        connectn_->NextTurn();
        UpdateUi();
    }
    // Reset mode and cursor
    convert_disk_mode_ = false;
    ui->centralwidget->setCursor(Qt::ArrowCursor);
    animation_ip_ = false;
}

void ConnectNWindow::EndRoundDialog(const QString &winner, int round)
{
    turn_timer_->stop();
    if (end_of_round_) {
        // Update End of round message and show
        end_of_round_->setWinner(winner, round);
        end_of_round_->show();
    } else {
        end_of_round_= new EndOfRound{winner, round, this};
    }
    end_of_round_->setModal(true);
    end_of_round_->exec();
}

void ConnectNWindow::EndRoundDialog(int round)
{
    turn_timer_->stop();
    if (end_of_round_) {
        end_of_round_->setDraw(round);
        end_of_round_->show();
    } else {
        end_of_round_= new EndOfRound{round, this};
    }
    end_of_round_->setModal(true);
    end_of_round_->exec();
}

void ConnectNWindow::EndGameDialog(const QString &winner)
{
    turn_timer_->stop();
    if (end_of_game_) {
        // update winner and show
        end_of_game_->setWinner(winner);
        end_of_game_->show();
    } else {
        end_of_game_ = new EndOfGame(winner, this);
    }
    end_of_game_->setModal(true);
    end_of_game_->exec();
    hide();
    startmenu->show();
    startmenu->setModal(true);
    if (startmenu->exec() == QDialog::Accepted) {
        switch (startmenu->get_choice()) {
        case StartMenu::Choice::NewGame:
            StartNewGame(startmenu->getNewGameParameters());
            show();
            break;
        case StartMenu::Choice::LoadGame:
            LoadGame(startmenu->get_filename());
            show();
            break;
        case StartMenu::Choice::ViewLeaderboard:
            break;
        }
    } else {
        QApplication::quit();
    }
}

/*
 * Updates the Ui turn timer only if an animation is not in progress.
 * When time runs out, HandleTimeOut routine is called.
 */
void ConnectNWindow::UpdateTurnTimer()
{
    if (animation_ip_) { return; }
    time_remaining_--;
    ui->progressBar->setValue(time_remaining_);
    if (time_remaining_ <= 0) {
        turn_timer_->stop();
        QString current_player = QString::fromStdString(connectn_->WhoseTurnName());
        QString next_player = QString::fromStdString(connectn_->get_p1()->get_name()) == current_player ?
                                  QString::fromStdString(connectn_->get_p2()->get_name()) : QString::fromStdString(connectn_->get_p1()->get_name());
        HandleTimeOut(next_player);
    }
}

void ConnectNWindow::HandleRoundWin(int winner)
{
    connectn_->IncrScore(winner);
    QString winner_name{};
    Player* winner_player = nullptr;
    Player* loser_player = nullptr;

    if (winner == 1) {
        winner_player = connectn_->get_p1();
        loser_player = connectn_->get_p2();
    } else {
        winner_player = connectn_->get_p2();
        loser_player = connectn_->get_p1();
    }

    winner_name = QString::fromStdString(winner_player->get_name());

    if ((connectn_->get_score()[winner]) >= WIN_GAME_SCORE) {
        HandleGameWin(QString::fromStdString(winner_player->get_name()),
                      QString::fromStdString(loser_player->get_name()));
        return;
    }

    // Delay dialog to allow disk animation to be cleaned up and Qt to reset its internal
    // timer. Without this, the next animation is not timed correctly.
    QTimer::singleShot(0, this, [this, winner_name]() {
        EndRoundDialog(winner_name, connectn_->get_round());
        connectn_->NextRound();
        UpdateUi();
    });
}

void ConnectNWindow::HandleGameWin(const QString &winner, const QString &loser)
{
    UpdateLeaderboard(winner, loser);
    EndGameDialog(winner);
}

void ConnectNWindow::HandleDraw()
{
    connectn_->IncrScore(0);
    // Delay dialog to allow disk animation to be cleaned up and Qt to reset its internal
    // timer. Without this, the next animation is not timed correctly.
    QTimer::singleShot(0, this, [this]() {
        EndRoundDialog(connectn_->get_round());
        connectn_->NextRound();
        UpdateUi();
    });
}

/*
 * Creates the OutOfTime dialog, which will appear for three seconds. After it closes,
 * the game advances to the next turn, the timer is reset, and the Ui is updated.
 */
void ConnectNWindow::HandleTimeOut(const QString &next_player)
{
    qDebug() << "Out of time!";
    if (out_of_time_) {
        out_of_time_->ResetTimer(next_player);
        out_of_time_->show();
    } else {
        out_of_time_ = new OutOfTime{next_player, this};
    }
    out_of_time_->setModal(true);
    out_of_time_->exec();
    connectn_->NextTurn();
    ResetTimer();
    UpdateTextUi();
    return;
}

int ConnectNWindow::GetGlobalUid()
{
    QString path = GetDataDirectory();
    QString filepath{path + "/globaluid.txt"};
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filepath;
    }

    QTextStream in(&file);
    QString guid{in.readLine()};
    qDebug() << "got guid: " << guid;
    return guid.toInt();
}

void ConnectNWindow::SaveGlobalUid(int global_uid)
{
    QString path = GetDataDirectory();
    QString filepath{path + "/globaluid.txt"};
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filepath;
    }

    QTextStream out(&file);
    out << QString::number(global_uid) + "\n";
    qDebug() << "saved global uid: " << global_uid;
    file.close();
}

void ConnectNWindow::UpdateLeaderboard(const QString &winner, const QString &loser)
{
    QVector<Leaderboard::LeaderboardEntry> entries{Leaderboard::LoadLeaderboard()};

    // See if winner/loser have entries
    // If so, update them
    // If not, add them
    bool winner_found{false}, loser_found{false};
    for (auto &entry : entries) {
        if (entry.name == winner) {
            entry.games_played++;
            entry.games_won++;
            entry.win_perc = entry.games_won / static_cast<double>(entry.games_played);
            winner_found = true;
        }
        if (entry.name == loser) {
            entry.games_played++;
            entry.win_perc = entry.games_won / static_cast<double>(entry.games_played);
            loser_found = true;
        }
    }

    if (!winner_found) {
        Leaderboard::LeaderboardEntry winner_entry;
        winner_entry.name = winner;
        winner_entry.games_played = 1;
        winner_entry.games_won = 1;
        winner_entry.win_perc = 1.0;
        entries.push_back(winner_entry);
    }

    if (!loser_found) {
        Leaderboard::LeaderboardEntry loser_entry;
        loser_entry.name = loser;
        loser_entry.games_played = 1;
        loser_entry.games_won = 0;
        loser_entry.win_perc = 0.0;
        entries.push_back(loser_entry);
    }

    // Write back leaderboard to csv
    QString path{GetLeaderboardPath()};
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open leaderboard";
        return;
    }

    QTextStream out(&file);

    for (auto &entry : entries) {
        out << entry.name << ","
            << entry.games_played << ","
            << entry.games_won << ","
            << entry.win_perc << "\n";
    }

    file.close();
}

QString ConnectNWindow::GetDataDirectory() {
    QString path = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);

    // Create directory if it doesn't exist
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return path;
}

QString ConnectNWindow::GetGamesDirectory() {
    QString path = GetDataDirectory();
    path += "/games";
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path;
}

QString ConnectNWindow::GetLeaderboardPath()
{
    QString path = GetDataDirectory();
    path+= "/leaderboard";
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    path+= "/leaderboard.csv";
    return path;
}


void ConnectNWindow::on_SaveGameButton_clicked()
{
    animation_ip_ = true;
    bool ok;
    QString game_name = QInputDialog::getText(
        this,
        "Game name",
        "Enter game name",
        QLineEdit::Normal,
        QString::fromStdString(connectn_->get_name()),
        &ok);
    if (ok) {
        // Write game to csv
        connectn_->set_name(game_name.toStdString());
        std::string game_data{connectn_->Stringify()};

        QString path = GetGamesDirectory();
        QString filepath{path + "/" + game_name + ".csv"};
        QFile file(filepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file for writing:" << filepath;
            return;
        }

        QTextStream out(&file);
        out << QString::fromStdString(game_data) + "\n";
        file.close();
        qDebug() << "File saved to: " << filepath;

        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Continue Game?",
            "Game saved! Continue playing?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            // Show start menu
            hide();
            startmenu->show();
            startmenu->setModal(true);
            if (startmenu->exec() == QDialog::Accepted) {
                switch (startmenu->get_choice()) {
                    case StartMenu::Choice::NewGame:
                        StartNewGame(startmenu->getNewGameParameters());
                        show();
                        break;
                    case StartMenu::Choice::LoadGame:
                        LoadGame(startmenu->get_filename());
                        show();
                        break;
                    case StartMenu::Choice::ViewLeaderboard:
                        break;
                }
            }
            else {
                QApplication::quit();
            }
        }
    }
    animation_ip_ = false;
}

void ConnectNWindow::on_ClearRowButtonP1_clicked()
{
    // If clicker is not p1, button has no effect
    if (connectn_->whose_turn() != 1) { return; }

    // If other modes activated, button has no effect
    if (clear_col_mode_ || convert_disk_mode_) { return; }

    // If player cannot clear a row, then button has no effect
    if (!connectn_->get_whose_turn()->can_clear_row()) { return; }

    clear_row_mode_ = !clear_row_mode_;

    if (clear_row_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_ClearColumnButtonP1_clicked()
{
    if (connectn_->whose_turn() != 1) { return; }
    if (clear_row_mode_ || convert_disk_mode_) { return; }
    if (!connectn_->get_whose_turn()->can_clear_col()) { return; }

    clear_col_mode_ = !clear_col_mode_;

    if (clear_col_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_ConvertDiskButtonP1_clicked()
{
    if (connectn_->whose_turn() != 1) { return; }
    if (clear_row_mode_ || clear_col_mode_) { return; }
    if (!connectn_->get_whose_turn()->can_convert()) { return; }

    convert_disk_mode_ = !convert_disk_mode_;

    if (convert_disk_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_ClearRowButtonP2_clicked()
{
    // If clicker is not p2, button has no effect
    if (connectn_->whose_turn() != 2) { return; }

    // If other modes activated, button has no effect
    if (clear_col_mode_ || convert_disk_mode_) { return; }

    // If player cannot clear a row, then button has no effect
    if (!connectn_->get_whose_turn()->can_clear_row()) { return; }

    clear_row_mode_ = !clear_row_mode_;

    if (clear_row_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_ClearColumnButtonP2_clicked()
{
    if (connectn_->whose_turn() != 2) { return; }
    if (clear_row_mode_ || convert_disk_mode_) { return; }
    if (!connectn_->get_whose_turn()->can_clear_col()) { return; }

    clear_col_mode_ = !clear_col_mode_;

    if (clear_col_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_ConvertDiskButtonP2_clicked()
{
    if (connectn_->whose_turn() != 2) { return; }
    if (clear_row_mode_ || clear_col_mode_) { return; }
    if (!connectn_->get_whose_turn()->can_convert()) { return; }

    convert_disk_mode_ = !convert_disk_mode_;

    if (convert_disk_mode_) {
        ui->centralwidget->setCursor(Qt::CrossCursor);
    }
    else {
        ui->centralwidget->setCursor(Qt::ArrowCursor);
    }
}

void ConnectNWindow::on_QuitGameButton_clicked()
{
    animation_ip_ = true;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Quit game",
        "Are you sure you want to quit this game?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // Show start menu
        hide();
        startmenu->show();
        if (startmenu->exec() == QDialog::Accepted) {
            switch (startmenu->get_choice()) {
            case StartMenu::Choice::NewGame:
                StartNewGame(startmenu->getNewGameParameters());
                show();
                break;
            case StartMenu::Choice::LoadGame:
                LoadGame(startmenu->get_filename());
                show();
                break;
            case StartMenu::Choice::ViewLeaderboard:
                break;
            }
        }
        else {
            qDebug() << "quitting application";
            QApplication::quit();
        }
    }
    qDebug() << "leaving on_QuitGameButton_clicked...";
    animation_ip_ = false;
}


void ConnectNWindow::on_EndRoundButton_clicked()
{
    animation_ip_ = true;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Forfeit round",
        "Are you sure you want to forfeit this round?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        Player *winner{connectn_->whose_turn() == 1 ? connectn_->get_p2() : connectn_->get_p1()};
        int winner_num{winner->get_num()};
        HandleRoundWin(winner_num);
    }
    animation_ip_ = false;
}

