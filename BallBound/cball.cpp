#include <QPainter>
#include "cball.h"

float g = -9.8f;
float boxSize = 0;

void CBall::UpDate(float dt, float maxPos)
{
	boxSize = maxPos;
	Vector2f latesSpeed = m_Vel;

	// óéâ∫
	m_Vel.y += g * dt;
	m_Pos.y += (latesSpeed.y + m_Vel.y) * dt / 2;

	m_Pos.x += m_Vel.x * dt;

	// ê⁄ínîªíË
	if ((m_Pos.y - m_radius) < -0.0f)
	{
		m_Vel.y = -m_Vel.y * 0.95f;
		m_Pos.y = m_radius;
	}

	if (boxSize < (m_Pos.x + m_radius))
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = boxSize - m_radius;
	}
	
	if ((m_Pos.x - m_radius) < -boxSize)
	{
		m_Vel.x = -m_Vel.x;
		m_Pos.x = -boxSize + m_radius;
	}
}

void CBall::draw(QPainter &painter)
{
	// íPà ïœä∑
	int r = m_radius * 1000;
	Vector2 position{ (int)(m_Pos.x * 1000), (int)(m_Pos.y * 1000) };
	// ï`âÊ
	painter.setPen(QPen(col, 2, Qt::SolidLine, Qt::FlatCap));
	painter.drawEllipse((position.x - r), (position.y - r), r * 2, r * 2);
}

void CBall::setInitialValue(Vector2f initialPos, Vector2f speed)
{
	// ë„ì¸
	m_Pos = initialPos;
	m_Vel = speed;
}

void CBall::setBall(int r, Qt::GlobalColor color)
{
	m_radius = (float)r / 1000;
	col = color;
}