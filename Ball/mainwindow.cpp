#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "variable.h"
#include "ball.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	startTimer(1000 / fps);

	// ボールそれぞれに速度を設定する。
	m_balls[0].Set(2.0f,  Qt::red);
	m_balls[1].Set(1.25f, Qt::blue);
	m_balls[2].Set(2.5f,  Qt::yellow);
	m_balls[3].Set(1.6f,  Qt::white);
	m_balls[4].Set(2.83f, Qt::magenta);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	/* ウィンドウサイズの取得 */
	Vector2 winSize;
	winSize.x = width();
	winSize.y = height();

	/* 背景塗りつぶし */
	QPainter painter(this);
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, winSize.x, winSize.y);

	/* グリッド */
	int gridSpan = 50;
	int gridNumber = 8;
	int gridWidth = gridSpan * gridNumber / 2;

	/* 画面サイズの決定 */
	int Size2;
	if (winSize.x < winSize.y) Size2 = winSize.x;
	if (winSize.x > winSize.y) Size2 = winSize.y;

	/* ペンの設定 */
	painter.setViewport(winSize.x / 2, winSize.y / 2, Size2, Size2);
	painter.setWindow(0, 0, (gridSpan * gridNumber), -(gridSpan * gridNumber));
	painter.setBrush(Qt::NoBrush);
	painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap));

	/* 描画 */

	// グリッドの描画
	int gridTop		=  gridWidth;
	int gridBottom	= -gridWidth;
	int gridLeft	= -gridWidth;
	int gridRight	=  gridWidth;
	// 縦
	for (int x = 0; x <= gridNumber; x++)
	{
		// どこからどこまで引くか計算
		int xx = gridLeft + gridSpan * x;
		painter.drawLine(xx, gridTop, xx, gridBottom);
	}
	// 横
	for (int y = 0; y <= gridNumber; y++)
	{
		// どこからどこまで引くか計算
		int yy = gridBottom + gridSpan * y;

		painter.drawLine(gridLeft, yy, gridRight, yy);
	}

	// 描画
	for (int i = 0; i < _countof(m_balls); i++)
	{
		m_balls[i].Draw(painter, gridWidth, gridSpan);
	}
}

void MainWindow::timerEvent(QTimerEvent *)
{
	for (int i = 0; i < _countof(m_balls); i++)
	{
		m_balls[i].Update(1.0f / (float)fps);
	}
	update();
}