#include <cmath>
#include "variable.h"

//-----------------------------------------------------------------------------
// ��_�Ԃ̋��������߂�
float Vector2f::GetDistance(Vector2f &vt)
{
	float dx = (vt.x - x);
	float dy = (vt.y - y);
	float dis = std::sqrt((dx * dx) + (dy * dy));
	return dis;
}

//-----------------------------------------------------------------------------
// Rect ����ɂ���
void Rect::Empty(void)
{
	minX = maxX = minY = maxY = 0.0f;
}

//-----------------------------------------------------------------------------
// Rect ���󂩔��肷��
bool Rect::isEmpty(void)
{
	return (maxX <= minX) || (maxY <= minY);
}

//-----------------------------------------------------------------------------
// Rect �����Ŋg������
void Rect::Expand(Vector2f &pos, float r)
{
	Vector2f sphereMin{ pos.x - r, pos.y - r };
	Vector2f sphereMax{ pos.x + r, pos.y + r };

	if (isEmpty())
	{
		// ���̃o�E���_����������`�Ƃ��đ��
		minX = sphereMin.x;
		maxX = sphereMax.x;
		minY = sphereMin.y;
		maxY = sphereMax.y;
	}
	else
	{
		// ��`���g������
		if (sphereMin.x < minX) minX = sphereMin.x;
		if (maxX < sphereMax.x) maxX = sphereMax.x;

		if (sphereMin.y < minY) minY = sphereMin.y;
		if (maxY < sphereMax.y) maxY = sphereMax.y;
	}
}

//-----------------------------------------------------------------------------
// Rect ���m���d�Ȃ��Ă��邩����
bool Rect::isOverlapped(Rect &rect)
{
	bool outside =
		(rect.maxX < minX) || (maxX < rect.minX) ||
		(rect.maxY < minY) || (maxY < rect.minY);
	return !outside;
}