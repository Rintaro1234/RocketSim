#include "pch.h"
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	// メモリリークを探すための機能
	// _CrtSetBreakAlloc(6594);
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
