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

	// �{�[�����ꂼ��ɑ��x��ݒ肷��B
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
	/* �E�B���h�E�T�C�Y�̎擾 */
	Vector2 winSize;
	winSize.x = width();
	winSize.y = height();

	/* �w�i�h��Ԃ� */
	QPainter painter(this);
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, winSize.x, winSize.y);

	/* �O���b�h */
	int gridSpan = 50;
	int gridNumber = 8;
	int gridWidth = gridSpan * gridNumber / 2;

	/* ��ʃT�C�Y�̌��� */
	int Size2;
	if (winSize.x < winSize.y) Size2 = winSize.x;
	if (winSize.x > winSize.y) Size2 = winSize.y;

	/* �y���̐ݒ� */
	painter.setViewport(winSize.x / 2, winSize.y / 2, Size2, Size2);
	painter.setWindow(0, 0, (gridSpan * gridNumber), -(gridSpan * gridNumber));
	painter.setBrush(Qt::NoBrush);
	painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap));

	/* �`�� */

	// �O���b�h�̕`��
	int gridTop		=  gridWidth;
	int gridBottom	= -gridWidth;
	int gridLeft	= -gridWidth;
	int gridRight	=  gridWidth;
	// �c
	for (int x = 0; x <= gridNumber; x++)
	{
		// �ǂ�����ǂ��܂ň������v�Z
		int xx = gridLeft + gridSpan * x;
		painter.drawLine(xx, gridTop, xx, gridBottom);
	}
	// ��
	for (int y = 0; y <= gridNumber; y++)
	{
		// �ǂ�����ǂ��܂ň������v�Z
		int yy = gridBottom + gridSpan * y;

		painter.drawLine(gridLeft, yy, gridRight, yy);
	}

	// �`��
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