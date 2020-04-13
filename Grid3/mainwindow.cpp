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
	/* �E�B���h�E�T�C�Y����� */
	Position Size;
	Size.x = width();
	Size.y = height();

	/* �y���̐ݒ� */
	QPainter painter(this);
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap)); // QPen(�F, ����, ���̎��, �[�̕���)

	/* �}�X�̐��Ƒ傫�� */
	int LineNumber = 8;
	int LineWidth = 50;

	/* �}�X�̈ʒu(��ɉ�ʒ��S��) */
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

	/* �`�� */
	// �c
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine((LineWidth * i), 0, (LineWidth * i), (LineWidth * LineNumber));
	}

	// ��
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine(0, (LineWidth * i), (LineWidth * LineNumber), (LineWidth * i));
	}
}