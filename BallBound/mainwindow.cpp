#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>

int fps = 60;
bool keyP = false;
bool KeySpace = false;

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

	// �����̃V�[�h��؂�ւ���
	std::srand(std::time(0));

	float left = (float)-m_maxPos / 1000.0f;
	float right = (float)m_maxPos / 1000.0f;
	float top = 0.6f;
	float bottom = 0.4f;
	float speedRange = 0.5f;

	for (int i = 0; i < _countof(m_balls); i++)
	{
		Vector2f Pos{ random(left, right), random(top, bottom)};
		Vector2f speed{ random(-speedRange, speedRange), 0};
		m_balls[i].setInitialValue(Pos, speed);
		m_balls[i].setBall(20, Qt::red, false);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
	/* ��ʔ͈͂���� */
	Vector2 size;
	size.x = width();
	size.y = height();

	/* �y���̒�` */
	QPainter painter(this);
	// �h��Ԃ�
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, size.x, size.y);
	// �r���[�|�[�g�̎w��
	int viewportSize;
	if (size.x < size.y) viewportSize = size.x -32;
	if (size.y < size.x) viewportSize = size.y -32;
	painter.setViewport(size.x / 2, size.y, viewportSize, viewportSize);

	/* �O���b�h */
	painter.setWindow(0, -16, m_gridSize, -m_gridSize);

	// �y���̐ݒ�
	painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::FlatCap));

	painter.save();

	// �O���b�h�`��
	painter.scale(1, -1);
	// �c��
	for (int i = 0; i <= m_gridNumber; i++)
	{
		int xx = -m_maxPos + m_gridSpan * i;
		painter.drawLine(xx, -m_maxPos * 2, xx, 0);
		painter.drawText(xx, 0, QString::number(xx) + "mm");
	}

	// ����
	for (int i = 0; i <= m_gridNumber; i++)
	{
		int yy = m_gridSpan * i;
		painter.drawLine(m_maxPos, -yy, -m_maxPos, -yy);
		painter.drawText(0, -yy, QString::number(yy) + "mm");
	}

	painter.restore();
	for (int i = 0; i < _countof(m_balls); i++)
	{
		m_balls[i].draw(painter);
	}
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	// P�L�[�������ꂽ��|�[�Y
	if (event->key() == Qt::Key_P)	   keyP = !keyP;
	if (event->key() == Qt::Key_Space) KeySpace = true;
}

void MainWindow::timerEvent(QTimerEvent *)
{
	// �|�[�Y����Ă��Ȃ�or�|�[�Y����Ă���ۂɃX�y�[�X�������ꂽ����s
	if (!keyP || KeySpace == true)
	{
		int div = 16;
		float div_dt = 1.0f / (fps * div);
		update();
		for (int a = 0; a <= div; a++)
		{
			// �ړ��v�Z(�R���W�����͖���)
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateMove(div_dt);
			}
			// �{�[�����m�̃R���W����
			for (int i = 0; i < _countof(m_balls); i++)
			{
				for(int j = i + 1; j < _countof(m_balls); j++)
				{
					m_balls[i].UpdateCollideBoll(div_dt, m_balls[j]);	
				}
			}
			// �{�[���Ə��̃R���W����
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateCollideWall(div_dt, (float)m_maxPos / 1000.0f);
			}
		}
		KeySpace = false;
	}
}

