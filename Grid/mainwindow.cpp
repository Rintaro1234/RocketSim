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

void MainWindow::paintEvent(QPaintEvent *) // �`��y�C���g
{
	Vector2 Pos;
	Pos.x = width();
	Pos.y = height();
	QPainter painter(this);
	/* ���C���ɂ��Ă̐ݒ� */
	painter.setRenderHint(QPainter::Antialiasing, true); // �A���`�G�C���A�X
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
	
	int LineNumber = Pos.x / 64; // ���C���̖{��
	int LineInterval = 0;
	for (int i = 0; i < LineNumber; i++) // �c����`��J�n
	{
		LineInterval += 64;
		painter.drawLine(LineInterval, 0, LineInterval, Pos.y);
	}
	LineNumber = Pos.y / 64; // ���C���̖{��
	LineInterval = 0;
	for (int i = 0; i < LineNumber; i++) // ������`��J�n
	{
		LineInterval += 64;
		painter.drawLine(0, LineInterval, Pos.x, LineInterval);
	}
}
