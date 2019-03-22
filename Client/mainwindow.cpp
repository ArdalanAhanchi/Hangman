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

    //Load the logo images into the labels.
    QPixmap img(LOGO_FILE);
    ui->logo_label->setPixmap(img.scaled(this->geometry().width(),
                            this->geometry().height(), Qt::KeepAspectRatio));
    ui->logo_label_2->setPixmap(img.scaled(this->geometry().width(),
                            this->geometry().height(), Qt::KeepAspectRatio));
    ui->logo_label_3->setPixmap(img.scaled(this->geometry().width(),
                           this->geometry().height(), Qt::KeepAspectRatio));

    //Initialize the buttons.
    connect(ui->login_button, SIGNAL(clicked()), this, SLOT(loginButtonClicked()));
    connect(ui->play_button, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(ui->logout_button, SIGNAL(clicked()), this, SLOT(logoutButtonClicked()));
    connect(ui->menu_button, SIGNAL(clicked()), this, SLOT(menuButtonClicked()));
    connect(ui->register_button, SIGNAL(clicked()), this, SLOT(registerButtonClicked()));
    connect(ui->rooms_button, SIGNAL(clicked()), this, SLOT(roomsButtonClicked()));
    connect(ui->rooms_join_button, SIGNAL(clicked()), this, SLOT(roomJoinButtonClicked()));
    connect(ui->rooms_back_button, SIGNAL(clicked()), this, SLOT(roomBackButtonClicked()));
    connect(ui->guessButton, SIGNAL(clicked()), this, SLOT(guessButtonClicked()));
    connect(ui->unreg_button, SIGNAL(clicked()), this, SLOT(unregButtonClicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::registerButtonClicked()
{
    cerr << "Register Clicked!" << endl;
    string user = ui->user_lineEdit->text().toStdString();
    string pass = ui->pass_lineEdit->text().toStdString();

    int ret = connection.reg(user, pass);                           //Ask server for registeration
    switch(ret)
    {
        case S_OK:
            ui->status_label->setText("Registration Successful.");
            break;

        case NOT_CONNECTED:
            disconnected();
            break;

        case S_REG_INVALID_USER:
            ui->status_label->setText("Registration Failed. Invalid Username.");
            break;

        case S_REG_INVALID_PASS:
            ui->status_label->setText("Registration Failed. Password Must be at least 8 characters long.");
            break;

        case S_REG_USERNAME_EXISTS:
            ui->status_label->setText("Registration Failed. Username alreay exists.");
            break;

        default:
             ui->status_label->setText("Registration Failed. Please Try Again.");
    }
}


void MainWindow::loginButtonClicked()
{
    cerr << "login Clicked!" << endl;
    string user = ui->user_lineEdit->text().toStdString();
    string pass = ui->pass_lineEdit->text().toStdString();

    int ret = connection.login(user, pass);                             //Try to login.
    switch(ret)
    {
        case S_OK:
            player = Player(user);
            ui->status_label->setText("Login Successful.");
            ui->stackedWidget->setCurrentIndex(1);
            break;

        case NOT_CONNECTED:
            disconnected();
            break;

        case S_AUTH_INVALID_USER:
            ui->status_label->setText("Login Failed. Invalid Username.");
            break;

        case S_AUTH_INVALID_PASS:
            ui->status_label->setText("Login Failed. Invalid Password.");
            break;

        default:
             ui->status_label->setText("Registration Failed. Please Try Again.");
    }
}

void MainWindow::playButtonClicked()
{
    qDebug() << "Play Clicked";
    ui->stackedWidget->setCurrentIndex(3);
    refreshGame();
}

void MainWindow::logoutButtonClicked()
{
    cerr << "Logout Clicked!" << endl;
    int ret = connection.logout();
    switch(ret)
    {
        case S_OK:
            ui->stackedWidget->setCurrentIndex(0);
            ui->status_label->setText("Logged Out Successfully.");
            ui->user_lineEdit->setText("");                             //Reset the password feilds.
            ui->pass_lineEdit->setText("");
            break;

        case NOT_CONNECTED:
            disconnected();
            break;

        default:
            disconnected();
            cerr << "Logout Failed." << endl;
    }
}

void MainWindow::menuButtonClicked()
{
    qDebug() << "Menu Clicked!";
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::roomsButtonClicked()
{
    cerr << "Rooms Clicked!" << endl;

    rooms.clear();
    int ret = connection.getRooms(rooms);
    switch(ret)
    {
        case S_OK:
            ui->stackedWidget->setCurrentIndex(2);
            ui->rooms_list->setText(QString(rooms.toString().c_str()));                         //Print the list of the rooms.
            ui->rooms_choice_line->setValidator(new QIntValidator(1, rooms.getSize(), this));   //Limit the number the user can put in the line edit.
            break;

        case NOT_CONNECTED:
            disconnected();
            break;

        default:
            cerr << "Fetching Rooms List Failed." << endl;
    }
}

void MainWindow::roomJoinButtonClicked()
{
    cerr << "Join Clicked!" << endl;

    int roomId = stoi(ui->rooms_choice_line->text().toStdString());
    cerr << "Chosen RoomId is: " << roomId << endl;
    cerr << "New game choice is: " << rooms.newGameChoice() << endl;

    if(roomId = rooms.newGameChoice())
    {
        int newRoom;
        switch(connection.host(newRoom))
        {
            case S_OK:
                ui->stackedWidget->setCurrentIndex(3);
                //ui->rooms_list->setText(QString(rooms.toString().c_str()));     //Print the list of the rooms.
                break;

            case NOT_CONNECTED:
                disconnected();
                break;

            default:
                cerr << "Hosting Failed." << endl;
        }
    }
}

void MainWindow::roomBackButtonClicked()
{
    cerr << "Back Clicked!";
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::unregButtonClicked()
{
    cerr << "Unregister Button Clicked" << endl;

    int ret = connection.unregister();
    switch(ret)
    {
        case S_OK:
            ui->stackedWidget->setCurrentIndex(0);
            ui->status_label->setText("Unregisterd Successfully.");
            break;

        case NOT_CONNECTED:
            disconnected();
            break;

        default:
            cerr << "Logout Failed." << endl;
    }
}

void MainWindow::guessButtonClicked()
{
    qDebug() << "Guess Clicked!";
    player.setLevel(player.getLevel() + 1);
    player.setPercentage(player.getPercentage() + 10);
    refreshGame();
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

void MainWindow::refreshGame()
{
    cerr << "Player Level: " << to_string(player.getLevel()) << " Player Perc: " << to_string(player.getPercentage());
    setFrame(player.getLevel());
    ui->levelText->setText(to_string(player.getLevel()).c_str());
    ui->percentText->setText(to_string(player.getPercentage()).c_str());

    if(player.getLevel() == 11)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::disconnected()
{
    cerr << "MainWindow: Disconnected from server." << endl;
    ui->stackedWidget->setCurrentIndex(0);
    ui->status_label->setText("Disconnected From Server. Please Try Again.");
}

