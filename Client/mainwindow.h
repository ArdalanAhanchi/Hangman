#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include "player.h"
#include "renderer.h"

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

public slots:
    void loginButtonClicked();
    void playButtonClicked();
    void joinButtonClicked();
    void logoutButtonClicked();
    void menuButtonClicked();

private:
    Ui::MainWindow* ui;
    Player* mainPlayer;
};

#endif // MAINWINDOW_H
