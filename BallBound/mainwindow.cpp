#include "pch.h"

// C/C++
#include <inttypes.h>
#include <ctime>
#include <stdlib.h>

// App
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cball.h"

// ボールの数
const int g_numBalls = 128;

// シミュレーションの時間分解能
const int fps = 60;

// 床となる放物線の係数
const FLOAT_T g_ParabolaFactor = 2.0f;

// シミュレーションのポーズ
bool g_Pause = false;
// ポーズ中のステップ実行
bool g_StepRun = false;
// 処理にかかった時間(0~1s)
int64_t sumTime = 0;
// QueryPerformance() の一秒はどれくらいか。
LARGE_INTEGER lpFrequency;

int numofbits5(int32_t bits);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	// 処理速度計測のためのカウンター周期を取得
	QueryPerformanceFrequency(&lpFrequency);
	
	// 乱数のシードを切り替える
	std::srand(std::time(0));

	// シミュレーションをリセット
	resetState();

	// マウスカーソルを、ドラッグ可能であることを示すハンドに変更
	setCursor(QCursor(Qt::OpenHandCursor));

	// 画面更新のタイマーをスタート
	ui->setupUi(this);
	startTimer(1000 / fps);
}

MainWindow::~MainWindow()
{
	if(m_simulator) delete m_simulator;
    delete ui;
}

// シミュレーションをリセットする
void MainWindow::resetState(void)
{
	if (m_simulator) delete m_simulator;
	FLOAT_T simSpaceW = (FLOAT_T)(2 * m_maxPos) / 1000.0f;
	m_simulator = new CSimulator(g_numBalls, simSpaceW, g_ParabolaFactor);
	m_simulator->resetState();
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
	if (m_simulator)
	{
		int numBalls;
		CBall *balls = m_simulator->GetBallArray(&numBalls);
		for (int i = 0; i < numBalls; i++)
		{
			balls[i].draw(painter);
		}
	}

	// FPS
	painter.setViewport(0, 0, width(), height());
	painter.setWindow(0, 0, width(), height());
	painter.setPen(Qt::red);
	painter.drawText(8, 16, "FPS:" + QString::number(m_fps, 'f', 1));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	// [P]キーが押されたらポーズ/解除
	if (event->key() == Qt::Key_P) g_Pause = !g_Pause;
	// [スペース]キーでコマ送り
	// ポーズ中でなければポーズ
	if (event->key() == Qt::Key_Space)
	{
		if (g_Pause)
		{
			g_StepRun = true;
		}
		else
		{
			g_Pause = true;
		}
	}
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
			// 処理時間計測
			LARGE_INTEGER start, end;
			QueryPerformanceCounter(&start);

			// 地形コリジョンのオフセットがある場合には、その移動も細分化する
			Vector2f stepFloorOffset = 
				m_floorOffset0 + (a + 1) * (floorOffset - m_floorOffset0) / stepCount;

			// シミュレーションを1ステップ進める
			if (m_simulator)
			{
				m_simulator->proceed(div_dt, stepFloorOffset, floorVel);
			}

			// 処理時間計測
			QueryPerformanceCounter(&end);
			sumTime += end.QuadPart - start.QuadPart;
			m_lpFrameCounter++;
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

	// 処理時間計測
	if (fps * 16 < m_lpFrameCounter)
	{
		double b = (double)(sumTime * 1000) / (double)m_lpFrameCounter / (double)lpFrequency.QuadPart;
		char buffer[128];
		sprintf_s(buffer, "ProcessingTime = %.3f (ms)\n", b);
		OutputDebugStringA(buffer);
		sumTime = 0;
		m_lpFrameCounter = 0;
	}

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

//-----------------------------------------------------------------------------
// クリックによるドラッグ開始
void MainWindow::mousePressEvent(QMouseEvent *event)
{
	QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
	m_dragMousePos0 = event->pos();
	m_draggingWnd = true;
}

//-----------------------------------------------------------------------------
// ウィンドウのドラッグ移動
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	// ドラッグ中でなければ何もしない
	if (!m_draggingWnd) return;

	// マウスの動きに追従してウィンドウをドラッグする。
	// マウス座標はウィンドウ左上からの相対座標。
	// ウィンドウ自体がマウスと一緒に移動するので、直前と同じ座標＋マウス移動量となる。
	QPoint dt = event->pos() - m_dragMousePos0;
	// 直前の座標＋マウス移動量でウィンドウを動かす
	move(pos() + dt);
}

//-----------------------------------------------------------------------------
// クリックによるドラッグ終了
void MainWindow::mouseReleaseEvent(QMouseEvent * /*event*/)
{
	QApplication::restoreOverrideCursor();
	m_draggingWnd = false;
}

// bit数を数えるプログラム
int numofbits5(int32_t bits)
{
	bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
	bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
	bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
	bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
	return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
}