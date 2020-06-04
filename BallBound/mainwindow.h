#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QtWidgets"
#include "cball.h"
#include "variable.h"
#include "parallelGroup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	void paintEvent(QPaintEvent *);
	void timerEvent(QTimerEvent *);
	void keyPressEvent(QKeyEvent *event);

protected:
	// シミュレーションをリセットする
	void resetState(void);

	int m_gridNumber = 12;
	int m_gridSpan = 50;
	int m_gridSize = m_gridNumber * m_gridSpan;
	int m_maxPos = m_gridSize / 2;
	int m_frameCounter = 0;
	int m_numParallelGroup;
	spaceGrid m_spaceGridA;
	ParallelGroup *m_parallelGroup;

	const int m_numBalls = 512;
	CBallPos *m_ballsPos;
	CBall *m_balls;

	// 起動時のウィンドウの位置(ウィンドウドラッグによるコリジョン遊びで参照)
	QPoint m_initWindowPos{ 0, 0 };
	// 初回のシミュレーション更新のフラグ(ウィンドウドラッグによるコリジョン遊びで参照)
	bool m_FirstUpdate = true;
	// 直前のシミュレーションでの、コリジョンのオフセット
	Vector2f m_floorOffset0{ 0.0f, 0.0f };

	int m_viewportSize = 0;

	// どの順番でコリジョン処理をするかの表
	int *m_CollisionTurn;

	// FPS計測用タイマー
	QTime m_fpsTime;
	// FPS計測用フレームカウンタ
	int m_fpsFrameCounter = 0;
	// FPS計測用、表示FPS値
	float m_fps = 0.0f;

	// 計測時間用のフレームカウンター
	int m_lpFrameCounter = 0;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
