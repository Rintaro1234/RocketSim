#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ball.h"

#include <QMainWindow>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int fps = 60;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
	void timerEvent(QTimerEvent *);
	void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;

	// ボールを三つ表示する
	CBall m_balls[5];
};
#endif // MAINWINDOW_H
