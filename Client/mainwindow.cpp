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
    username = user;

    int ret = connection.login(user, pass);                             //Try to login.
    switch(ret)
    {
        case S_OK:
            player = Player(username);
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
    switch(connection.joinRand(currRoomId))
    {
            case S_OK:
            {
                 cerr << "Joined room " << currRoomId << endl;

                 ui->stackedWidget->setCurrentIndex(3);
                 setWord();
                 updateWord();
                 refreshGame();
                 player = Player(username);
                        //ui->rooms_list->setText(QString(rooms.toString().c_str()));     //Print the list of the rooms.
                 break;
            }

            case NOT_CONNECTED:
            {
                disconnected();
                break;
            }

            default:
               cerr << "Join Failed." << endl;
               break;
    }
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
    if(roomId == rooms.newGameChoice())
    {
        int newRoom;
        switch(connection.host(newRoom))
        {
            case S_OK:
            {
                switch(connection.join(roomId))
                {
                    case S_OK:
                        cerr << "Joined room " << roomId << endl;
                        currRoomId = roomId;

                        ui->stackedWidget->setCurrentIndex(3);
                        refreshGame();
                        //ui->rooms_list->setText(QString(rooms.toString().c_str()));     //Print the list of the rooms.
                        break;

                    case NOT_CONNECTED:
                        disconnected();
                        break;

                    default:
                        cerr << "Join Failed." << endl;
                }

                break;
            }

            case NOT_CONNECTED:
                disconnected();
                break;

            default:
                cerr << "Hosting Failed." << endl;
        }
    }
    else
    {
        switch(connection.join(roomId))
        {
            case S_OK:
                cerr << "Joined room " << roomId << endl;
                currRoomId = roomId;

                ui->stackedWidget->setCurrentIndex(3);
                refreshGame();
                //ui->rooms_list->setText(QString(rooms.toString().c_str()));     //Print the list of the rooms.
                break;

            case NOT_CONNECTED:
                disconnected();
                break;

            default:
                cerr << "Join Failed." << endl;
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

void MainWindow::update()
{

}

void MainWindow::guessButtonClicked()
{
    qDebug() << "Guess Clicked!";
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
    //connection.update(currRoomId, player);

    string playerBoard;
    connection.getPlayerBoard(currRoomId, playerBoard);

    setFrame(player.getLevel());
    ui->levelText->setText(to_string(player.getLevel()).c_str());
    ui->percentText->setText(to_string(player.getPercentage()).c_str());
}

void MainWindow::disconnected()
{
    cerr << "MainWindow: Disconnected from server." << endl;
    ui->stackedWidget->setCurrentIndex(0);
    ui->status_label->setText("Disconnected From Server. Please Try Again.");
}

void MainWindow::setWord()
{
    string newWord;
    connection.getWord(currRoomId, newWord);

    word.clear();
    guessed.clear();
    for(int i = 0; i < newWord.size(); i++)
    {
        word[i] = newWord[i];
        guessed[i] = false;
    }
}

void MainWindow::updateWord()
{
    string out = "";
    for(int i = 0; i < word.size(); i++)
    {
        if(guessed[i])
            out += word[i];         //Print the word if its been guessed.
        else {
            out += " _ ";           //An underscore if not.
        }
    }
    ui->wordText->setText(out.c_str());
}

void MainWindow::guess(char guess)
{
    bool guessedRight = false;
    for(int i = 0; i < word.size(); i++)
    {
        if(!guessed[i])
        {
            if(guess == word[i])
            {
                guessed[i] = true;         //Set it to true if it was guessed.
                guessedRight = true;
            }
        }
    }
    if(!guessedRight)
    {
        player.setLevel(player.getLevel() + 1);
        if(player.getLevel() == NUMBER_OF_FRAMES)
            connection.lost(currRoomId);
    }
    else
    {
        float percentage = 0;
        for(int i = 0; i < guessed[i]; i++)
            if(guessed[i])
                percentage++;

        if(percentage == 100)
            connection.won(currRoomId);

        percentage /= guessed.size();
        percentage *= 100;
        player.setPercentage(percentage);

    }
}

