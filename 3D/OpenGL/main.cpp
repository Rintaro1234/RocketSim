#include <QApplication>
#include <qsurfaceFormat.h>
#include <GLWidget.h>

int main(int argc, char *argv[])
{
	QSurfaceFormat fmt;
	fmt.setVersion(4, 0); // OpenGL�̃o�[�W�������w��
	fmt.setProfile(QSurfaceFormat::CoreProfile); // coreProfile���g��
	QSurfaceFormat::setDefaultFormat(fmt); // �ȏ�̐ݒ��K�p����

    QApplication a(argc, argv);
	GLWidget w;
	w.show();
    return a.exec();
}
