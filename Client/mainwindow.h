#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

#include <QMainWindow>
#include <QDebug>
#include <stdlib.h>
#include <QIntValidator>

#include "player.h"
#include "constants.h"
#include "roomlist.h"
#include "connection.h"

#define LOGO_FILE ":/logo.png"
#define FRAME_FILES_PREFIX ":/frames/frame_"
#define NUMBER_OF_FRAMES 11

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setFrame(int level);

    void refreshGame();
    void setWord();
    void updateWord();
    void guess(char guess);

public slots:
    void loginButtonClicked();
    void playButtonClicked();
    void logoutButtonClicked();
    void menuButtonClicked();
    void registerButtonClicked();
    void roomsButtonClicked();
    void roomJoinButtonClicked();
    void roomBackButtonClicked();
    void guessButtonClicked();
    void unregButtonClicked();

    void disconnected();
    void update();

private:
    Ui::MainWindow* ui;
    Player player;
    Connection connection;
    RoomList rooms;
    int currRoomId;
    vector<char> word;
    vector<bool> guessed;
    string username;
};

#endif // MAINWINDOW_H
