#pragma once

#include "variable.h"

namespace Qt
{
	enum GlobalColor;
};

class QPainter;

class CBall
{
public:
	// 描画
	void draw(QPainter &painter);
	// 更新
	void Update(float dt, float maxPos, CBall other[], int arraySize, int myNumber);
	// 最初の位置
	void setInitialValue(Vector2f initialPos, Vector2f speed);
	// 色と半径
	void setBall(int r, Qt::GlobalColor color, bool a);
	// 落下と移動
	void move(float dt);

	Vector2f m_Pos{0, 0};
	Vector2f m_Vel{0, 0};
	Rect	 m_Rect{0, 0};

	float m_radius = 20;
	Qt::GlobalColor col;

	bool A = false;
};