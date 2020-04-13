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

void MainWindow::paintEvent(QPaintEvent *)
{
	/* ウィンドウサイズを入手 */
	Position Size;
	Size.x = width();
	Size.y = height();

	/* ペンの設定 */
	QPainter painter(this);
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap)); // QPen(色, 厚さ, 線の種類, 端の閉じ方)

	/* マスの数と大きさ */
	int LineNumber = 8;
	int LineWidth = 50;

	/* マスの位置(常に画面中心へ) */
	int Size2;

	if (Size.x < Size.y)
	{
		Size2 = Size.x;
		LineWidth = Size.x / LineNumber;
	}
	else
	{
		Size2 = Size.y;
		LineWidth = Size.y / LineNumber;
	}

	Position EmptySpace;
	EmptySpace.x = (Size.x / 2) - (LineWidth * LineNumber / 2);
	EmptySpace.y = (Size.y / 2) - (LineWidth * LineNumber / 2);

	painter.setViewport(EmptySpace.x, EmptySpace.y, Size2, Size2);
	painter.setWindow(QRect(0, 0, Size2, Size2));

	/* 描画 */
	// 縦
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine((LineWidth * i), 0, (LineWidth * i), (LineWidth * LineNumber));
	}

	// 横
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine(0, (LineWidth * i), (LineWidth * LineNumber), (LineWidth * i));
	}
}