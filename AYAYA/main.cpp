#include "mainwindow.hpp"
#include <QApplication>

#include <QFile>
#include <QTextStream>

#include <QDebug>

#include "player/extractors/ytdlextractor.hpp"
#include <QFutureWatcher>
#include <QUrl>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.show();
    w.init();

    return a.exec();
}
