#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.show();
    w.init();

    return a.exec();
}
