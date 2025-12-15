#include "leaderboard.h"
#include "startmenu.h"
#include "ui_startmenu.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QString>
#include <vector>

StartMenu::StartMenu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartMenu)
{
    ui->setupUi(this);
    qDebug() << "startmenu created";
}

StartMenu::~StartMenu()
{
    qDebug() << "startmenu destroyed";
    delete ui;
}

std::vector<QString> StartMenu::getNewGameParameters() const
{
    return std::vector<QString>{QString::number(n_), p1_name_, p2_name_};
}

void StartMenu::on_NewGameButton_clicked()
{
    bool ok;
    int value = QInputDialog::getInt(
        this,                   // Parent widget
        "Choose game size",     // Dialog title
        "Number of disks:",     // Label text
        4,                      // Default value
        4,                      // Minimum value
        10,                     // Maximum value
        1,                      // Step size
        &ok                     // Was OK clicked?
        );

    if (ok) {
        // User clicked OK, use 'value'
        n_ = value;
        ok = false;
        QString p1_name = QInputDialog::getText(
            this,
            "Player 1's name",
            "Enter player 1's name",
            QLineEdit::Normal,
            "Player 1",
            &ok);

        if (ok) {
            p1_name_ = p1_name;
            ok = false;
            QString p2_name = QInputDialog::getText(
                this,
                "Player 2's name",
                "Enter player 2's name",
                QLineEdit::Normal,
                "Player 2",
                &ok);

            if (ok) {
                p2_name_ = p2_name;
                choice_ = Choice::NewGame;
                this->accept();
            } else {
                // no p2 name
            }
        } else {
            // no p1 name
        }
    } else {
        // disks not chosen
    }
}


void StartMenu::on_LoadGameButton_clicked()
{
    QString path = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);

    // Create directory if it doesn't exist
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString games_path{path + "/games"};

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Load Game",
        games_path
        );

    qDebug() << "got filename:" << filename;

    if (!filename.isEmpty()) {
        // Open file
        filename_ = filename;
        choice_ = Choice::LoadGame;
        this->accept();
    }
}


void StartMenu::on_ViewLeaderboardButton_clicked()
{
    if (leaderboard_) {
        leaderboard_->show();
    } else {
        leaderboard_ = new Leaderboard{this};
    }
    leaderboard_->setModal(true);
    leaderboard_->exec();
}

