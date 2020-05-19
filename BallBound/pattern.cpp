#include "iostream"
using namespace std;

int main(void)
{
	int **CollisionTurns;
	int BullNumber = 4; // �����̂�

	// y����Ƀ{�[���̐������z����쐬
	CollisionTurns = new int*[BullNumber];
	// x����Ƀ{�[���̐������z����쐬
	for (int i = 0; i < BullNumber; i++) 
	{
		CollisionTurns[i] = new int[BullNumber];
	}

	// �E�΂߉������ɐ��������Ă���
	int k = 0;
	for (int i = 0; i < BullNumber / 2; i++)
	{
		//�@�����v�f�̑g�ݍ��킹�͂���Ȃ�����x������E�ɂ��炷
		int xx = i + 1;
		int yy = 0;
		for (int j = 0; j < BullNumber; j++)
		{
			// x���������炵�Ă��邽�߁A�z��ȏ�ɑ傫���Ȃ�B
			// ���̏ꍇ�Axx��yy�������Ayy��0�ɂ���B
			// ��������΁A�����̑g�ݍ��킹��������B
			if (BullNumber <= xx)
			{
				if (BullNumber / 2 < i + 2) break;
				xx = yy;
				yy = 0;
			}
			// �Ƃ肠������������̂Ƃ��̓e�L�g�[�Ȓl�����Ƃ�
			if (i % 2 == 0)
			{
				// �ŏ�(y = 0)�̗v�f���ł��邾���ŏ��Ɏ��s���������ߏ������l�����s
				// �����I�ɂ���邽�ߋ���or��Ŕ���
				if (j % 2 == 0)
				{
					CollisionTurns[yy][xx] = k;
				}
				else
				{
					CollisionTurns[yy][xx] = k + 1;
				}
			}
			else
			{
				CollisionTurns[yy][xx] = k;
			}
			// ���̏ꏊ�ֈړ�
			xx++;
			yy++;
		}
		// 1���ꂠ����2�i�ނ��߁{2����
		k = k + 2;
	}

	// �Ȃ������ɂ͎�荇��-1�����Ēu���A���Ƃŕϊ�
	for (int i = 0; i < BullNumber; i++)
	{
		CollisionTurns[i][i] = -1;
	}

	for (int i = 0; i < BullNumber / 2; i++)
	{
		int xx = i + 1;
		int yy = 0;
		for (int j = 0; j < BullNumber; j++)
		{
			if (BullNumber <= xx)
			{
				xx = yy;
				yy = 0;
			}
			CollisionTurns[xx][yy] = -1;
			xx++;
			yy++;
		}
	}

	// �\��
	for (int i = 0; i < BullNumber; i++)
	{
		for (int j = 0; j < BullNumber; j++)
		{
			if (CollisionTurns[i][j] == -1)
			{
				cout << "x" << " ,";
			}
			else
			{
				// �񌅈ȏゾ�ƕ\�����ɂ����Ȃ邽�ߏC��(�{���s�v)
				if(10 <= CollisionTurns[i][j])cout << CollisionTurns[i][j] << ",";
				if(CollisionTurns[i][j] < 10)cout << CollisionTurns[i][j] << " ,";
			}
		}
		cout << endl;
	}

	// �������J��
	delete CollisionTurns;
}