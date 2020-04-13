#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) // 描画ペイント
{
	Vector2 Pos;
	Pos.x = width();
	Pos.y = height();
	QPainter painter(this);
	/* ラインについての設定 */
	painter.setRenderHint(QPainter::Antialiasing, true); // アンチエイリアス
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
	
	int LineNumber = Pos.x / 64; // ラインの本数
	int LineInterval = 0;
	for (int i = 0; i < LineNumber; i++) // 縦線を描画開始
	{
		LineInterval += 64;
		painter.drawLine(LineInterval, 0, LineInterval, Pos.y);
	}
	LineNumber = Pos.y / 64; // ラインの本数
	LineInterval = 0;
	for (int i = 0; i < LineNumber; i++) // 横線を描画開始
	{
		LineInterval += 64;
		painter.drawLine(0, LineInterval, Pos.x, LineInterval);
	}
}
