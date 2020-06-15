#include <QApplication>
#include <qsurfaceFormat.h>
#include <GLWidget.h>

int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
	fmt.setVersion(4, 0); // OpenGLのバージョンを指定
	fmt.setProfile(QSurfaceFormat::CoreProfile); // coreProfileを使う
	QSurfaceFormat::setDefaultFormat(fmt); // 以上の設定を適用する

    QApplication a(argc, argv);
	GLWidget w;
	w.show();
    return a.exec();
}
