#include "mainwindow.h"

#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

	QLabel label("Hello QT", &w);

    w.show();
    return a.exec();
}
