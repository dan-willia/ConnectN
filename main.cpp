/* main.cpp
 *
 * The main driver for the QApplication implementation of ConnectN.
 *
 * On a.exec, main window is hidden and StartMenu dialog shows three options:
 * New Game, Load Game, and View Leaderboard.
 *
 * If New Game or Load Game selected, the dialog closes and the main window shows.
 * If Leaderboard is selected, the leaderboard dialog shows and the event loop restarts.
 *
 * If user exits from start menu, the application closes.
 *
 */

#include <QApplication>
#include <QLocale>

#include "connectn.h"
#include "connectnwindow.h"
#include "startmenu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("ConnectN");

    ConnectNWindow w;

    StartMenu start{&w};  // Dialog, parented to game window
    w.setStartMenu(&start);
    if (start.exec() == QDialog::Accepted) {
        switch (start.get_choice()) {
            case StartMenu::Choice::NewGame:
                w.StartNewGame(start.getNewGameParameters());
                w.show();
                break;
            case StartMenu::Choice::LoadGame:
                w.LoadGame(start.get_filename());
                w.show();
                break;
            case StartMenu::Choice::ViewLeaderboard:
                // dialog shows inside start menu
                break;
        }
        return a.exec();
    }

    return 0;  // User exited start menu

}
