#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
	void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;
};

class Vector2
{
public: float x, y;
};
#endif // MAINWINDOW_H
