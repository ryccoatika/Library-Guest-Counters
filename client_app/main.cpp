#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(QApplication::desktop()->screen()->width(),QApplication::desktop()->screen()->height());
    w.showFullScreen();
    return a.exec();
}
