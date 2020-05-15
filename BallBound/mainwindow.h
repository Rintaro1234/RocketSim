#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QtWidgets"
#include "cball.h"
#include "variable.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	void paintEvent(QPaintEvent *);
	void timerEvent(QTimerEvent *);
	void keyPressEvent(QKeyEvent *event);

protected:
	// �V�~�����[�V���������Z�b�g����
	void resetState(void);

	int m_gridNumber = 12;
	int m_gridSpan = 50;
	int m_gridSize = m_gridNumber * m_gridSpan;
	int m_maxPos = m_gridSize / 2;
	int m_frameCounter = 0;

	CBall m_balls[4];

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
