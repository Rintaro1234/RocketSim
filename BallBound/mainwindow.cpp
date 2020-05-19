#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>

int fps = 60;
// 床となる放物線の係数
FLOAT_T g_ParabolaFactor = 2.0f;
// シミュレーションのポーズ
bool g_Pause = false;
// ポーズ中のステップ実行
bool g_StepRun = false;

FLOAT_T random(FLOAT_T x0, FLOAT_T x1)
{
	return x0 + (x1 - x0) / 256.0f * (FLOAT_T)(std::rand() / (RAND_MAX / 256));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	startTimer(1000 / fps);

	// 乱数のシードを切り替える
	std::srand(std::time(0));

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
	// ボールの初期位置と初速度を乱数で決める
	// (ついでに色も)
	FLOAT_T left = (FLOAT_T)-m_maxPos / 1000.0f;
	FLOAT_T right = (FLOAT_T)m_maxPos / 1000.0f;
	FLOAT_T top = 0.6f;
	FLOAT_T bottom = 0.4f;
	FLOAT_T speedRange = 0.5f;
	Qt::GlobalColor ColorTable[] = { Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::yellow };
	for (int i = 0; i < _countof(m_balls); i++)
	{
		Vector2f Pos{ random(left, right), random(top, bottom) };
		Vector2f speed{ random(-speedRange, speedRange), 0 };
		m_balls[i].setInitialValue(Pos, speed);
		Qt::GlobalColor color = ColorTable[i % _countof(ColorTable)];
		m_balls[i].setBall(10, color, 1);
	}
	//m_balls[0].setBall(40, ColorTable[0], 4);
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
	if (size.x < size.y) m_viewportSize = size.x -32;
	if (size.y < size.x) m_viewportSize = size.y -32;
	painter.setViewport(size.x / 2, size.y, m_viewportSize, m_viewportSize);
	// ウィンドウドラッグによる地形コリジョンのオフセットを、描画範囲のオフセットとして利用
	// 画面下側に bottom_margin [mm]のマージンを設ける
	const FLOAT_T bottom_margin = 0.016f;
	Vector2 windowOffset{ (int)(1000.0f * m_floorOffset0.x), (int)(1000.0f * (m_floorOffset0.y - bottom_margin)) };
	painter.setWindow(windowOffset.x, windowOffset.y, m_gridSize, -m_gridSize);

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
	FLOAT_T x0 = (FLOAT_T)-m_maxPos / 1000.0f;
	FLOAT_T y0 = g_ParabolaFactor * (x0 * x0) + m_floorOffset0.y;
	for (int i = -m_maxPos+1; i <= m_maxPos; i++)
	{
		FLOAT_T x1 = (FLOAT_T)i / 1000.0f;
		FLOAT_T y1 = g_ParabolaFactor * (x1 * x1) + m_floorOffset0.y;
		int offsetX = (int)(m_floorOffset0.x * 1000.0f);
		painter.drawLine((int)(1000.0f*x0) + offsetX, (int)(1000.0f*y0), (int)(1000.0f*x1) + offsetX, (int)(1000.0f*y1));
		x0 = x1;
		y0 = y1;
	}
	// 横線
	{
		int offsetX = (int)(m_floorOffset0.x * 1000.0f);
		int offsetY = (int)(m_floorOffset0.y * 1000.0f);
		painter.drawLine((-m_maxPos + offsetX), offsetY, (-m_maxPos + offsetX), 2 * m_maxPos + offsetY);
		painter.drawLine((m_maxPos + offsetX), offsetY, (m_maxPos + offsetX), 2 * m_maxPos + offsetY);
	}
	// ボール
	for (int i = 0; i < _countof(m_balls); i++)
	{
		m_balls[i].draw(painter);
	}

	// FPS
	painter.setViewport(0, 0, width(), height());
	painter.setWindow(0, 0, width(), height());
	painter.setPen(Qt::red);
	painter.drawText(8, 16, "FPS:" + QString::number(m_fps, 'f', 1));
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
	// ウィンドウの移動量(ドット)から、地形コリジョンおよび、表示範囲のオフセット([m])を計算
	Vector2f floorOffset{ 0.0f, 0.0f };
	if (m_FirstUpdate)
	{
		// 初回ではウィンドウの位置を記録
		m_initWindowPos = pos();
		m_FirstUpdate = false;
		// FPS計測用に、初回の時間を代入
		m_fpsTime.start();
	}
	else
	{
		// 二回目以降、オフセットを計算
		QPoint windowMove = pos() - m_initWindowPos;
		floorOffset.x = (FLOAT_T)(m_gridSize * windowMove.x()) / (FLOAT_T)(m_viewportSize * 1000);
		floorOffset.y = (FLOAT_T)(-m_gridSize * windowMove.y()) / (FLOAT_T)(m_viewportSize * 1000);
	}
	// 地形コリジョンの移動速度を計算([m/s])
	Vector2f floorVel = (floorOffset - m_floorOffset0) * (FLOAT_T)(fps);

	// FPS計測用、経過時間を取得
	int fpsElapsedTime = m_fpsTime.elapsed();
	if (1000 <= fpsElapsedTime)
	{
		m_fpsTime.restart();
	}

	// ポーズされていないorポーズされている際にスペースが押されたら実行
	if (!g_Pause || g_StepRun)
	{
		// 衝突のすり抜けを防止するため、1フレームをさらに細分化してシミュレーションする。
		int div = 16;
		FLOAT_T div_dt = 1.0f / (FLOAT_T)(fps * div);

		// ポーズ中のステップ実行の場合には、1フレームをさらに細分化したステップで進める。
		bool stepRun = (g_Pause && g_StepRun);
		int stepCount = (stepRun? 1: div);

		for (int a = 0; a < stepCount; a++)
		{
			// 移動計算(コリジョンは無視)
			//#pragma omp parallel for
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateMove(div_dt);
			}
			// ボール同士のコリジョン
			for (int i = 0; i < _countof(m_balls); i++)
			{
				#pragma omp parallel for
				for(int j = i + 1; j < _countof(m_balls); j++)
				{
					m_balls[i].UpdateCollideBoll(div_dt, m_balls[j]);	
				}
			}
			// ボールと床のコリジョン
			// 地形コリジョンのオフセットがある場合には、その移動も細分化する
			Vector2f co = m_floorOffset0 + (a + 1) * (floorOffset - m_floorOffset0) / stepCount;
			// 並行実行(マルチスレッド)
			//#pragma omp parallel for
			for (int i = 0; i < _countof(m_balls); i++)
			{
				m_balls[i].UpdateCollideWall(div_dt, (FLOAT_T)m_maxPos / 1000.0f, g_ParabolaFactor, co, floorVel);
			}
		}
	}
	g_StepRun = false;

	// コリジョンのオフセットを次のシミュレーションへ送る
	m_floorOffset0 = floorOffset;

	// FPS計測
	if (1000 <= fpsElapsedTime)
	{
		m_fps = 1000.0f * (float)m_fpsFrameCounter / (float)fpsElapsedTime;
		m_fpsFrameCounter = 0;
	}
	m_fpsFrameCounter++;

	// デバッグ表示
	m_frameCounter++;
	if (fps < m_frameCounter)
	{
		// デバッグ
		char buffer[64];
		sprintf_s(buffer, "Ft.x = %.3f, Ft.y = %.3f\n", CBall::sm_Ft.x, CBall::sm_Ft.y);
		OutputDebugStringA(buffer);
		CBall::sm_Ft = Vector2f();
		m_frameCounter = 1;
	}

	// 画面を即座に再描画
	repaint();
}

