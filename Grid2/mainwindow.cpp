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
	int LineWidth = 0;

	if (Size.x < Size.y) 
	{
		LineWidth = Size.x / LineNumber;
	}
	else 
	{
		LineWidth = Size.y / LineNumber;
	}

	/* �}�X�̈ʒu(��ɉ�ʒ��S��) */
	Position EmptySpace;
	EmptySpace.x = (Size.x / 2) - (LineWidth * LineNumber / 2);
	EmptySpace.y = (Size.y / 2) - (LineWidth * LineNumber / 2);

	/* �`�� */
	// �c
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine((LineWidth * i + EmptySpace.x), EmptySpace.y, (LineWidth * i + EmptySpace.x), (LineWidth * LineNumber + EmptySpace.y));
	}

	// ��
	for (int i = 0; i <= LineNumber; i++)
	{
		painter.drawLine(EmptySpace.x, (LineWidth * i + EmptySpace.y), (LineWidth * LineNumber + EmptySpace.x), (LineWidth * i + EmptySpace.y));
	}
}

