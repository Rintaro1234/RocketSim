﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

#include "variable.h"

class CBall;
class CBallPos;
class CSimulator;

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

// クライアント領域をドラッグしてのウィンドウ移動
protected:
	// クリックによるウィンドウのドラッグ開始
	virtual void mousePressEvent(QMouseEvent *event);
	// クリックによるウィンドウのドラッグ終了
	virtual void mouseReleaseEvent(QMouseEvent *event);
	// ウィンドウのドラッグ移動
	virtual void mouseMoveEvent(QMouseEvent *event);

	// ドラッグ中フラグ
	bool m_draggingWnd = false;
	// ドラッグ開始時点でのマウス座標
	QPoint m_dragMousePos0;

	// 起動時のウィンドウの位置(ウィンドウドラッグによるコリジョン遊びで参照)
	QPoint m_initWindowPos{ 0, 0 };
	// 初回のシミュレーション更新のフラグ(ウィンドウドラッグによるコリジョン遊びで参照)
	bool m_FirstUpdate = true;
	// 直前のシミュレーションでの、コリジョンのオフセット
	Vector2f m_floorOffset0{ 0.0f, 0.0f };

protected:
	// シミュレーションをリセットする
	void resetState(void);

	// シミュレーション実行
	CSimulator *m_simulator = nullptr;

	// グリッドのパラメータ
	int m_gridNumber = 12;
	int m_gridSpan = 50;
	int m_gridSize = m_gridNumber * m_gridSpan;
	int m_maxPos = m_gridSize / 2;
	int m_frameCounter = 0;
	int m_numParallelGroup;
	int m_viewportSize = 0;

	// どの順番でコリジョン処理をするかの表
	//int *m_CollisionTurn;

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
