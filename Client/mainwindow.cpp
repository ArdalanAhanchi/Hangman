#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Init to the login page.
    ui->stackedWidget->setCurrentIndex(0);

    //Init frame and player.
    setFrame(0);
    mainPlayer = new Player("DefaultName");

    //Load the logo images into the labels.
    QPixmap img(LOGO_FILE);
    ui->logo_label->setPixmap(img.scaled(this->geometry().width(),
                            this->geometry().height(), Qt::KeepAspectRatio));
    ui->logo_label_2->setPixmap(img.scaled(this->geometry().width(),
                            this->geometry().height(), Qt::KeepAspectRatio));

    //Initialize the buttons.
    connect(ui->login_button, SIGNAL(clicked()), this, SLOT(loginButtonClicked()));
    connect(ui->play_button, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(ui->join_button, SIGNAL(clicked()), this, SLOT(joinButtonClicked()));
    connect(ui->logout_button, SIGNAL(clicked()), this, SLOT(logoutButtonClicked()));
    connect(ui->menu_button, SIGNAL(clicked()), this, SLOT(menuButtonClicked()));


    //game g;
    //g.update(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainPlayer;
}


void MainWindow::loginButtonClicked()
{
    qDebug() << "login Clicked!";
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::playButtonClicked()
{
    qDebug() << "Play Clicked";
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::joinButtonClicked()
{
    qDebug() << "Join Clicked!";
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::logoutButtonClicked()
{
    qDebug() << "Logout Clicked!";
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::menuButtonClicked()
{
    qDebug() << "Menu Clicked!";
    ui->stackedWidget->setCurrentIndex(1);
    setFrame(10);
}

void MainWindow::setFrame(int level)
{
    //Check for invalid frame numbers.
    if(level >= NUMBER_OF_FRAMES)
    {
        cerr << "ERROR: Invalid level requested." << endl;
        return;
    }

    //Create a filename based on the given level.
    string fileName = FRAME_FILES_PREFIX;
    fileName.append(to_string(level));

    //Load the pixmap from file and set the frame.
    QPixmap img(fileName.c_str());
    ui->frame_img->setPixmap(img.scaled(ui->frame_img->width(),
                            ui->frame_img->height(), Qt::KeepAspectRatio));
}

