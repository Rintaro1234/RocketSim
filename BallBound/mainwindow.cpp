#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>

int fps = 60;

float random(float x0, float x1)
{
	return x0 + (x1 - x0) / 256.0f * (float)(std::rand() / (RAND_MAX / 256));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	startTimer(1000 / fps);

	// 乱数のシードを切り替える
	std::srand(std::time(0));

	float left = (float)-m_maxPos / 1000.0f;
	float right = (float)m_maxPos / 1000.0f;
	float top = 0.6f;
	float bottom = 0.4f;
	float speedRange = 0.5f;

	for (int i = 0; i < _countof(cBall); i++)
	{
		Vector2f Pos{ random(left, right), random(top, bottom)};
		Vector2f speed{ random(-speedRange, speedRange), 0};
		cBall[i].setInitialValue(Pos, speed);
		cBall[i].setBall(10, Qt::red);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	/* 画面範囲を入手 */
	Vector2 size;
	size.x = width();
	size.y = height();

	/* ペンの定義 */
	QPainter painter(this);
	// 塗りつぶし
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, size.x, size.y);
	// ビューポートの指定
	int viewportSize;
	if (size.x < size.y) viewportSize = size.x -32;
	if (size.y < size.x) viewportSize = size.y -32;
	painter.setViewport(size.x / 2, size.y, viewportSize, viewportSize);

	/* グリッド */
	painter.setWindow(0, -16, m_gridSize, -m_gridSize);

	// ペンの設定
	painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::FlatCap));

	painter.save();

	// グリッド描画
	painter.scale(1, -1);
	// 縦軸
	for (int i = 0; i <= m_gridNumber; i++)
	{
		int xx = -m_maxPos + m_gridSpan * i;
		painter.drawLine(xx, -m_maxPos * 2, xx, 0);
		painter.drawText(xx, 0, QString::number(xx) + "mm");
	}

	// 横軸
	for (int i = 0; i <= m_gridNumber; i++)
	{
		int yy = m_gridSpan * i;
		painter.drawLine(m_maxPos, -yy, -m_maxPos, -yy);
		painter.drawText(0, -yy, QString::number(yy) + "mm");
	}

	painter.restore();
	for (int i = 0; i < _countof(cBall); i++)
	{
		cBall[i].draw(painter);
	}
}

void MainWindow::timerEvent(QTimerEvent *)
{
	update();
	for (int i = 0; i < _countof(cBall); i++)
	{
		cBall[i].UpDate(1.0f / fps, (float)m_maxPos / 1000);
	}
}

