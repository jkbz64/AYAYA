#include "mainwindow.hpp"
#include <QApplication>

#include <QFile>
#include <QTextStream>

#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_NativeWindows);

    MainWindow w;

    w.show();
    w.init();

    return a.exec();
}
