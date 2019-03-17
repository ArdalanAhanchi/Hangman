#include "mainwindow.h"
#include <QApplication>

#define TITLE "Hangman"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(TITLE);
    w.show();

    return a.exec();
}
