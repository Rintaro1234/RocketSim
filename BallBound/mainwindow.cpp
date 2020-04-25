#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>

int fps = 60;
// 床となる放物線の係数
float g_ParabolaFactor = 2.0f;
// シミュレーションのポーズ
bool g_Pause = false;
// ポーズ中のステップ実行
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

	// シミュレーションをリセット
	resetState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// シミュレーションをリセットする
void MainWindow::resetState(void)
{
	// 乱数のシードを切り替える
	std::srand(std::time(0));

	// ボールの初期位置と初速度を乱数で決める
	// (ついでに色も)
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

	// 曲線
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
	// [P]キーが押されたらポーズ
	if (event->key() == Qt::Key_P) g_Pause = !g_Pause;
	// [スペース]キーでコマ送り
	if (event->key() == Qt::Key_Space) g_StepRun = true;

	// [O]キーでリセット
	if (event->key() == Qt::Key_O)
	{
		// シミュレーションをリセット
		resetState();
		// ポーズ状態で再開
		g_Pause = true;
	}
}

void MainWindow::timerEvent(QTimerEvent *)
{
	// ポーズされていないorポーズされている際にスペースが押されたら実行
	if (!g_Pause || g_StepRun)
	{
		// 衝突のすり抜けを防止するため、1フレームをさらに細分化してシミュレーションする。
		int div = 16;
		float div_dt = 1.0f / (float)(fps * div);

		// ポーズ中のステップ実行の場合には、1フレームをさらに細分化したステップで進める。
		bool stepRun = (g_Pause && g_StepRun);
		int stepCount = (stepRun? 1: div);
		for (int a = 0; a <= stepCount; a++)
		{
			// 移動計算(コリジョンは無視)
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateMove(div_dt);
			}
			// ボール同士のコリジョン
			for (int i = 0; i < _countof(m_balls); i++)
			{
				for(int j = i + 1; j < _countof(m_balls); j++)
				{
					m_balls[i].UpdateCollideBoll(div_dt, m_balls[j]);	
				}
			}
			// ボールと床のコリジョン
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateCollideWall(div_dt, (float)m_maxPos / 1000.0f, g_ParabolaFactor);
			}
		}
	}
	g_StepRun = false;

	// Qtフレームワークに更新を通知
	update();
}

