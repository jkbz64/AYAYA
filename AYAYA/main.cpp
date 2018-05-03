#include "mainwindow.hpp"
#include <QApplication>

#include <QFile>
#include <QTextStream>

#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

// Poor windows users having lidl default windows style
#if defined(Q_OS_WIN) || defined(_WIN32)
    QFile f(":/qdarkstyle/style.qss");
    if (!f.exists()) {
        printf("Unable to set stylesheet, file not found\n");
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
#endif
    w.show();

    return a.exec();
}
