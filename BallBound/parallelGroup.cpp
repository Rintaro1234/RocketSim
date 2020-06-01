#include <iostream>
#include <string>
#include <algorithm>
#include "parallelGroup.h"
using namespace std;

int Wave(int hz, int i);

// �[���l�ߓ񌅂�10�i���������Ԃ�
string ato2i(int x)
{
	string str;
	if (x < 10) str = "0";
	str += to_string(x);
	return str;
}

//-----------------------------------------------------------------------------
// �����̂͂ݏo���ɑ΂���ܕԂ����l�����āA�O���b�h�̃C���f�b�N�X��ϊ�����
void GetWrapIndex(int N, int x, int y, int *xx, int *yy)
{
	// �c���Ƃ��ɁA�v�f��N�����Ő܂�Ԃ�
	x = (x % N);
	y = (y % N);

	// �O���b�h�̏㔼�����w���悤�ɁA���b�v����
	if (y <= x)
	{
		*xx = x;
		*yy = y;
	}
	else
	{
		*xx = y;
		*yy = x;
	}
}

// �΂߃��C���̕��񏈗��O���[�v�����邯��
// @param N �{�[���̐�
// @param CollisionPairTable[][] �R���W����������s��2�̃{�[���̃y�A��\���A�񎟌��z��̃O���b�h
// @param k ���C���擪�̈ʒu(x���W)
// @param pairNo ���̃O���[�v�ԍ�
// @return ���̃O���[�v�ԍ�
int fingCollisionPairSub(int N, int **CollisionPairTable, int k, int pairNo);

// ���������������
void deleteArray(ParallelGroup *ParallelArray, int N)
{
	for (int i = 0; i < N; i++)
	{
		delete[] ParallelArray[i].array;
	}
	delete[] ParallelArray;
}


//-----------------------------------------------------------------------------
ParallelGroup *parallelGenerator(int numBalls, int *numParallelGroup)
{
	// �R���W����������s��2�̃{�[���̃y�A���A�񎟌��z��̃O���b�h�ŕ\���B
	// �ϐ����̐����́A����ڂ̕��񏈗��O���[�v�Ŕ��肷�邩�B
	// (CollisionPairTable[2][3] = 5)
	// �́A2�Ԗڂ�3�Ԗڂ̃{�[�����A5��ڂ̕��񏈗��O���[�v�ŃR���W�����`�F�b�N����A�̈Ӗ��B
	// Note:
	// �O���b�h�̑Ίp�����͏d���B�������͎g��Ȃ��B
	// ��) CollisionPairTable[4][6] �� CollisionPairTable[6][4] �͓����y�A�BCollisionPairTable[6][4] �͎g��Ȃ��B
	int **CollisionPairTable = new int*[numBalls];
	for (int i = 0; i < numBalls; i++)
	{
		CollisionPairTable[i] = new int[numBalls];
		for (int j = 0; j < numBalls; j++) CollisionPairTable[i][j] = -1;
	}

	// �΂߂̃��C�������ɏ�������
	int numGroup = 0;
	for (int k = 1; k <= numBalls / 2; k++)
	{
		numGroup = fingCollisionPairSub(numBalls, CollisionPairTable, k, numGroup);
	}

	// ���ۂɎg����悤�ɕϊ�
	ParallelGroup *parallelGroup = new ParallelGroup[numGroup];
	for (int i = 0; i < numGroup; i++)
	{
		// ���s�����ł���y�A�̐�
		int groupNo = 0;

		// ���s�����ł���y�A�̐��𒲂ׂ�
		for (int x = 0; x < numBalls; x++)
		{
			for (int y = 0; y < numBalls; y++)
			{
				if (CollisionPairTable[y][x] == i) groupNo++;
			}
		}
		parallelGroup[i].array = new CollisionPair[groupNo];
		parallelGroup[i].numPair = groupNo;
		// �y�A�����Ă���
		for (int a = 0; a < groupNo;)
		{
			for (int x = 0; x < numBalls; x++)
			{
				for (int y = 0; y < numBalls; y++)
				{
					if (CollisionPairTable[y][x] == i)
					{
						parallelGroup[i].array[a].idx0 = x;
						parallelGroup[i].array[a].idx1 = y;
						a++;
					}
				}
			}
		}
	}

	// �������J��
	for (int y = 0; y < numBalls; y++)
	{
		delete[] CollisionPairTable[y];
	}
	delete[] CollisionPairTable;

	*numParallelGroup = numGroup;
	return parallelGroup;
}

//-----------------------------------------------------------------------------
// ��������C���f�b�N�X�ɑ΂��Ď����I�ȋ�`�g(0��1)��Ԃ��ϐ�
// @param L 0��������1���A�����鐔
// @param i �C���f�b�N�X
int Wave(int L, int i)
{
	// ��`�g�̎����� 2L �Ȃ̂ŁA���͈̔͂ɐ܂�Ԃ��B
	i = i % (L * 2);
	// ��`�g�̑O��(i < L)��0�A�㔼(L <= i)��1
	return (i < L) ? 0 : 1;
}

//-----------------------------------------------------------------------------
// �΂߃��C���̕��񏈗��O���[�v�����邯��
// @param N �{�[���̐�
// @param CollisionPair[][] �R���W����������s��2�̃{�[���̃y�A��\���A�񎟌��z��̃O���b�h
// @param k ���C���擪�̈ʒu(x���W)
// @param pairNoBase ���̃O���[�v�ԍ�
// @return ���̃O���[�v�ԍ�
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNoBase)
{
	// ���ꎟ����ɏ��Ԃ����Ă���\�ɑ������
	int *pair = new int[N];
	// ��ɂ���(-1)
	for (int i = 0; i < N; i++)
	{
		pair[i] = -1;
	}

	// �l�����Ă���
	while (true)
	{
		// �`�F�b�N���X�^�[�g����{�[���̑g�ݍ��킹��������
		int groupID = pairNoBase;
		int p;
		for (p = 0; p < N; p++)
		{
			// ���`�F�b�N�̃y�A����X�^�[�g
			if (pair[p] == -1)
			{
				// ���̃y�A�Ɋ܂܂��{�[�����Q�Ƃ����ʂ̃y�A�����`�F�b�N�A
				// �������͕ʂ̕��񏈗��O���[�v�Ȃ�OK�B
				// �����łȂ��Ȃ�ʂ̕��񏈗��O���[�v�ŃX�^�[�g����B
				int pn = p - k;
				if (pn < 0) pn = pn + N;
				int pp = p + k;
				if (N <= pp) pp = pp - N;
				while (true)
				{
					if ((/*(pair[pn] == -1) || */(pair[pn] != groupID)) &&
						(/*(pair[pp] == -1) || */(pair[pp] != groupID)))
					{
						break;
					}
					groupID++;
				}
				break;
			}
		}
		// �S�Ẵy�A���`�F�b�N�ς݂Ȃ犮��
		if (p == N) break;

		for (int i = 0; i < N; i++)
		{
			// ������R���W�����`�F�b�N���܂߂悤�Ƃ���A���񏈗��O���[�v�̔ԍ�
			int gid = groupID + Wave(1, i);
			// ���̃y�A�Ɋ܂܂��{�[�����Q�Ƃ���鎟�̃y�A�����`�F�b�N�A
			// �������͕ʂ̕��񏈗��O���[�v�Ȃ�OK�B
			int pp = p + k;
			if (N <= pp) pp = pp - N;

			if ((pair[p] == -1) && (pair[pp] != gid))
			{
				pair[p] = gid;
			}
			else
			{
				// �_���������ꍇ�ɂ́A���̃X�L�������I�����āA���ɃX�^�[�g�\�ȃy�A��T�������B
				break;
			}
			// ���̃y�A�Ɋ܂܂�{�[�����Q�Ƃ���鎟�̃y�A�֐i�ށB
			p = pp;
		}
	}

	// x����y�����߁A�����Ɉꎞ�z��̏������Ă���
	int maxGroupID = 0;
	int x = k - 1, y = 0;
	for (int i = 0; i < N; i++)
	{
		x = x + 1;
		y = x - k;
		// �{�[���̐����炠�ӂꂵ�Ă����ꍇ�Čv�Z
		if (N <= x)
		{
			k = N - k;
			x = y;
			y = x - k;
		}

		// �܂�Ԃ����g�ݍ��킹�����łɕʂ̕��񏈗��O���[�v�ɓ����Ă���\��������B
		// ���̂悤�ȏꍇ�͏㏑�����Ȃ��B
		if (CollisionPair[y][x] < 0)
		{
			CollisionPair[y][x] = pair[i];
			if (maxGroupID < pair[i]) maxGroupID = pair[i];
		}
	}
	delete[] pair;

	return maxGroupID + 1;
}