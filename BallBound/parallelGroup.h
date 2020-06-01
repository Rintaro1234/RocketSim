#pragma once

// �R���W�������`�F�b�N����A2�̃{�[���̑g�ݍ��킹
struct CollisionPair
{
	int idx0 = -1;
	int idx1 = -1;
};

//-----------------------------------------------------------------------------
// ���������ŕ��񏈗��\�ȁA�R���W�����`�F�b�N���s���{�[���̑g�ݍ��킹�̃O���[�v
struct ParallelGroup
{
	CollisionPair *array = nullptr;
	int numPair = 0;
};

ParallelGroup *parallelGenerator(int numBalls, int *numParallelGroup);
void deleteArray(ParallelGroup *ParallelArray, int N);