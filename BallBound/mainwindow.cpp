#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>

int fps = 60;
// ���ƂȂ�������̌W��
float g_ParabolaFactor = 2.0f;
// �V�~�����[�V�����̃|�[�Y
bool g_Pause = false;
// �|�[�Y���̃X�e�b�v���s
bool g_StepRun = false;

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

	// �V�~�����[�V���������Z�b�g
	resetState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// �V�~�����[�V���������Z�b�g����
void MainWindow::resetState(void)
{
	// �����̃V�[�h��؂�ւ���
	std::srand(std::time(0));

	// �{�[���̏����ʒu�Ə����x�𗐐��Ō��߂�
	// (���łɐF��)
	float left = (float)-m_maxPos / 1000.0f;
	float right = (float)m_maxPos / 1000.0f;
	float top = 0.6f;
	float bottom = 0.4f;
	float speedRange = 0.5f;
	Qt::GlobalColor ColorTable[] = { Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::yellow };
	for (int i = 0; i < _countof(m_balls); i++)
	{
		Vector2f Pos{ random(left, right), random(top, bottom) };
		Vector2f speed{ random(-speedRange, speedRange), 0 };
		m_balls[i].setInitialValue(Pos, speed);
		Qt::GlobalColor color = ColorTable[i % _countof(ColorTable)];
		m_balls[i].setBall(20, color, 1);
	}
	m_balls[0].setBall(40, ColorTable[0], 4);
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

	// �Ȑ�
	painter.scale(1, -1);
	painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::FlatCap));
	float x0 = (float)-m_maxPos / 1000.0f;
	float y0 = g_ParabolaFactor * (x0 * x0);
	for (int i = -m_maxPos+1; i <= m_maxPos; i++)
	{
		float x1 = (float)i / 1000.0f;
		float y1 = g_ParabolaFactor * (x1 * x1);
		painter.drawLine((int)(1000.0f*x0), (int)(1000.0f*y0), (int)(1000.0f*x1), (int)(1000.0f*y1));
		x0 = x1;
		y0 = y1;
	}

	for (int i = 0; i < _countof(m_balls); i++)
	{
		m_balls[i].draw(painter);
	}
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	// [P]�L�[�������ꂽ��|�[�Y
	if (event->key() == Qt::Key_P) g_Pause = !g_Pause;
	// [�X�y�[�X]�L�[�ŃR�}����
	if (event->key() == Qt::Key_Space) g_StepRun = true;

	// [O]�L�[�Ń��Z�b�g
	if (event->key() == Qt::Key_O)
	{
		// �V�~�����[�V���������Z�b�g
		resetState();
		// �|�[�Y��ԂōĊJ
		g_Pause = true;
	}
}

void MainWindow::timerEvent(QTimerEvent *)
{
	// �|�[�Y����Ă��Ȃ�or�|�[�Y����Ă���ۂɃX�y�[�X�������ꂽ����s
	if (!g_Pause || g_StepRun)
	{
		// �Փ˂̂��蔲����h�~���邽�߁A1�t���[��������ɍו������ăV�~�����[�V��������B
		int div = 16;
		float div_dt = 1.0f / (float)(fps * div);

		// �|�[�Y���̃X�e�b�v���s�̏ꍇ�ɂ́A1�t���[��������ɍו��������X�e�b�v�Ői�߂�B
		bool stepRun = (g_Pause && g_StepRun);
		int stepCount = (stepRun? 1: div);
		for (int a = 0; a <= stepCount; a++)
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
				m_balls[i].UpdateCollideWall(div_dt, (float)m_maxPos / 1000.0f, g_ParabolaFactor);
			}
		}
	}
	g_StepRun = false;

	// Qt�t���[�����[�N�ɍX�V��ʒm
	update();
}

