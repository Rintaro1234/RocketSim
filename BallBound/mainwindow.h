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

	int m_gridNumber = 12;
	int m_gridSpan = 50;
	int m_gridSize = m_gridNumber * m_gridSpan;
	int m_maxPos = m_gridSize / 2;

	CBall cBall[5];

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
